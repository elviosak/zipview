#include "mainwindow.h"
#include <QThread>

#define SCROLLWIDTH 600



MainWindow::MainWindow(int argc, char *argv[], QWidget *parent)
    : QMainWindow(parent)
{
    setAcceptDrops(true);
    setWindowIcon(QIcon(":/zipview"));
    s = new QSettings("zipview", "zipview");
    openFolder = s->value("openFolder", QString()).toString();

    wid = new QWidget;
    vbox = new QVBoxLayout(wid);
    vbox->setContentsMargins(0,0,0,0);
    wid->setContentsMargins(0,0,0,0);
    area = new Area;
    area->setWidgetResizable(true);
    area->setMinimumWidth(200);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidget(wid);
    setCentralWidget(area);
    auto bigFont = font();
    if (font().pixelSize() > 0)
        bigFont.setPixelSize(font().pixelSize() * 1.5);
    if (font().pointSize() > 0)
        bigFont.setPointSize(font().pointSize() * 1.5);
    defaultLabel = new QLabel("Select or Drop file");
    defaultLabel->setFont(bigFont);
    vbox->addWidget(defaultLabel, 0, Qt::AlignCenter);
    for (int i = 0; i < 7; ++i) {
        auto lbl = new QLabel;
        vbox->addWidget(lbl, 0, Qt::AlignHCenter);
        allLabels.append(lbl);
    }
    endLabel = new QLabel("End");
    endLabel->setFont(bigFont);
    vbox->addWidget(endLabel, 0, Qt::AlignHCenter);
    endLabel->hide();
    resizeTimer = new QTimer;
    resizeTimer->setInterval(100);
    resizeTimer->setSingleShot(true);
    selectBtn = new QPushButton("Open");
    pageComboBox = new QComboBox;
    pageComboBox->setEditable(false);
    currentPage = new QLabel;

    auto tbWidget = new QWidget;
    tbWidget->setContentsMargins(0,0,0,0);
    auto tbLayout = new QHBoxLayout(tbWidget);
    tbLayout->setContentsMargins(0,0,0,0);
    tbLayout->addWidget(selectBtn);
    tbLayout->addWidget(pageComboBox, 1);
    tbLayout->addWidget(currentPage);
    auto tb = new QToolBar;
    tb->addWidget(tbWidget);
    tb->setMovable(false);
    addToolBar(Qt::ToolBarArea::BottomToolBarArea, tb);

    auto dock = new QDockWidget;
    thumbView = new QListView;
    thumbView->setDragDropMode(QListView::NoDragDrop);
    thumbView->setSelectionMode(QAbstractItemView::SingleSelection);
    thumbView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    thumbView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    thumbView->setItemDelegate(new Delegate);
    int w = TWIDTH + thumbView->verticalScrollBar()->sizeHint().width() + 8;
    thumbView->setFixedWidth(w);
    model = new QStandardItemModel;
    thumbView->setModel(model);
    dock->setWidget(thumbView);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    auto dockCheck = new QCheckBox;
    dockCheck->setChecked(true);
    dockCheck->setText("Preview");
    connect(dockCheck, &QCheckBox::toggled, this, [dock] (bool checked) {
        dock->setVisible(checked);
    });
    tbLayout->insertWidget(0 ,dockCheck);
    resize(SCROLLWIDTH + TWIDTH, 800);

    connect(thumbView, &QListView::clicked, this, [this] (const QModelIndex &index) {
        loadFromIndex(index.row());
    });
    connect(area->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::scrollChanged);
    connect(area, &Area::resized, resizeTimer, QOverload<>::of(&QTimer::start));
    connect(resizeTimer, &QTimer::timeout, this, &MainWindow::resizeTimeout);
    connect(pageComboBox, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::loadFromIndex);
    connect(selectBtn, &QPushButton::clicked, this, [=] {
        QString f = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        openFolder,
                                                        tr("zip/cbz files (*.zip *.cbz)"));
        if (!f.isEmpty()) {
            QFileInfo info(f);
            openFolder = info.absolutePath();
            s->setValue("openFolder", openFolder);
            loadFile(f);
        }
    });
    if (argc > 1) {
        auto fileName = QString::fromUtf8(argv[1]);
        if (QFile::exists(fileName)) {
            QTimer::singleShot(100, this, [this, fileName] {
                loadFile(fileName);
            });
        }
    }
}

MainWindow::~MainWindow()
{
}

int MainWindow::getImageWidth()
{
    int w = qMax(196, area->size().width());
    int imgWidth = w - 2;
    return imgWidth;
}

void MainWindow::resizeTimeout()
{
    loadPixmaps();
}

void MainWindow::loadFile(QString f)
{
    fileList.clear();
    pageComboBox->clear();
    QFileInfo info(f);
    if (!info.exists() && !(info.suffix().toLower() == "zip" || info.suffix().toLower() == "cbz")) {
        QMessageBox::warning(this, "Invalid File", QString("file: %1\n is not valid").arg(f));
        return;
    }
    defaultLabel->hide();
    QuaZip zip(f);
    zipName = f;
    zip.open(QuaZip::Mode::mdUnzip);

//    (?<!) - Negative look behind assertion
//    matches end of string when NOT preceded by '/'
//    to remove dir entries
    QRegularExpression re("(?<!/)$");
    fileList = zip.getFileNameList().filter(re);
    fileList.sort();
    int total = fileList.count();
    currentPage->setText(QString::number(total) + " pages.");
    fileName = info.fileName();
    setWindowTitle(QString("zipview - (%1) %2").arg(QString::number(total), info.fileName()));
    zip.close();

    pageComboBox->addItems(fileList);
    loadFromIndex();
    endLabel->show();
    auto thread = QThread::create([this] {this->loadThumbs();});
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void MainWindow::loadThumbs()
{
    model->clear();
    QuaZip zip(zipName);
    zip.open(QuaZip::Mode::mdUnzip);
    QuaZipFile file2(&zip);
    for (int i = 0; i < fileList.count(); ++i) {
        auto fName = fileList.at(i);
        zip.setCurrentFile(fName);
        file2.open(QIODevice::ReadOnly);
        auto img = QImage::fromData(file2.readAll());
        QPixmap pix = QPixmap::fromImage(img)
                .scaled(TWIDTH,THEIGHT, Qt::KeepAspectRatio,Qt::SmoothTransformation);
        auto item = new QStandardItem;
        item->setData(pix, Qt::UserRole + 1);
        item->setToolTip(fName);
        model->appendRow(item);
        file2.close();
    }
    zip.close();
}

void MainWindow::loadFromIndex(int index)
{
    allPixmaps.clear();
    int start, end;
    if (fileList.count() > 7) {
        if (index <= 1) {
            end = 6;
            start = 0;
        }
        else {
            end = qMin (index + 5, fileList.count() -1);
            start = end - 6;
        }
    }
    else {
        start = 0;
        end = fileList.count() -1;
    }
    int offset = index - start;

    pageComboBox->setCurrentIndex(index);
    QuaZip zip(zipName);
    zip.open(QuaZip::Mode::mdUnzip);
    QuaZipFile file(&zip);
    for (int i = start; i <= end; ++i) {
        zip.setCurrentFile(fileList.at(i));
        file.open(QIODevice::ReadOnly);
        auto img = QImage::fromData(file.readAll());
        auto pix = QPixmap::fromImage(img);
        allPixmaps.append(pix);
        file.close();
        lastIndex = i;
    }
    zip.close();
    while (allPixmaps.count() < 7) {
        allPixmaps.append(QPixmap());
    }
    loadPixmaps();
    int scrollValue = 0;
    for (int i = 0; i < offset; ++i) {
        scrollValue += allLabels.at(i)->sizeHint().height();
    }
    area->verticalScrollBar()->setValue(scrollValue);

}

void MainWindow::loadNext()
{
    if (!(lastIndex + 1 < fileList.count()))
        return;

    lastIndex++;
    auto fileName = fileList.at(lastIndex);
    QuaZip zip(zipName);
    zip.open(QuaZip::Mode::mdUnzip);
    QuaZipFile file(&zip);
    zip.setCurrentFile(fileName);
    file.open(QIODevice::ReadOnly);
    auto img = QImage::fromData(file.readAll());
    auto pix = QPixmap::fromImage(img);
    allPixmaps.removeFirst();
    allPixmaps.append(pix);
    file.close();
    zip.close();

    int h = allLabels.first()->height();
    int val = area->verticalScrollBar()->value();
    loadPixmaps();
    area->verticalScrollBar()->setValue(val - h);
}

void MainWindow::loadPrev()
{
    if (lastIndex < 7)
        return;

    lastIndex--;
    int first = lastIndex - 6;
    auto fileName = fileList.at(first);
    QuaZip zip(zipName);
    zip.open(QuaZip::Mode::mdUnzip);
    QuaZipFile file(&zip);
    zip.setCurrentFile(fileName);
    allPixmaps.removeLast();
    file.open(QIODevice::ReadOnly);
    auto img = QImage::fromData(file.readAll());
    auto pix = QPixmap::fromImage(img);
    allPixmaps.prepend(pix);
    file.close();
    zip.close();


    int val = area->verticalScrollBar()->value();
    loadPixmaps();
    int h = allLabels.first()->contentsRect().height();
    area->verticalScrollBar()->setValue(val + h);
}

void MainWindow::loadPixmaps()
{
    if (allPixmaps.count() == 0)
        return;
    auto w = getImageWidth();
    for (int i = 0; i < 7; ++i) {
        auto pix = allPixmaps.at(i);
        auto lbl = allLabels.at(i);
        if (pix.isNull())
            lbl->clear();
        else
            lbl->setPixmap(pix.scaledToWidth(w, Qt::SmoothTransformation));
    }
}

void MainWindow::scrollChanged(int /*v*/)
{
    if (allLabels.at(5)
        && allLabels.at(5)->geometry()
            .translated(0, wid->geometry().y())
            .intersects(area->geometry()))
        loadNext();
    else if (allLabels.at(0)
        && allLabels.at(0)->geometry()
           .translated(0, wid->geometry().y())
           .intersects(area->geometry()))
        loadPrev();

    updatePage();
}

void MainWindow::updatePage()
{
    int page = 0;
    int intersected = 0;
    for (int i = 0; i < allLabels.count(); ++i) {
        QRect lblGeo = allLabels.at(i)->geometry();
        lblGeo.translate(0, wid->geometry().y());
        QRect r = lblGeo.intersected(area->geometry());
        if (r.height() > intersected) {
            intersected = r.height();
            page = i;
        }
    }
    page = page + lastIndex - qMin(fileList.count() - 1, 6) ;
    pageComboBox->setCurrentIndex(page);
    auto msg = QString("page %1 of %2").arg(QString::number(page + 1), QString::number(fileList.count()));
    currentPage->setText(msg);
    auto item = model->item(page);
    auto index = model->indexFromItem(item);
    thumbView->setCurrentIndex(index);
    setWindowTitle(QString("zipview - (%1/%2) %3")
                   .arg(QString::number(page + 1)
                        , QString::number(fileList.count())
                        , fileName));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() || e->mimeData()->hasText())
        e->accept();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QString fileName;
    if (e->mimeData()->hasUrls())
        fileName = e->mimeData()->urls().first().toLocalFile();
    else if (e->mimeData()->hasText())
        fileName = e->mimeData()->text().trimmed();
    if (!fileName.isEmpty())
        loadFile(fileName);

}
