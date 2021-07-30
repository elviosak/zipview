#include "mainwindow.h"

#define WIDTH 600

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
    area = new QScrollArea;
    area->setWidgetResizable(true);
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

    connect(area->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::scrollChanged);
    resizeTimer = new QTimer;
    resizeTimer->setInterval(100);
    resizeTimer->setSingleShot(true);
    connect(resizeTimer, &QTimer::timeout, this, &MainWindow::resizeTimeout);
    currentPage = new QLabel;
    pageComboBox = new QComboBox;
    pageComboBox->setEditable(false);
    connect(pageComboBox, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::loadFromIndex);
    currentFile = new QLabel;
    selectBtn = new QPushButton("Open");
    statusBar()->addWidget(selectBtn);
    statusBar()->addWidget(pageComboBox);
    statusBar()->addWidget(currentPage);
    statusBar()->addWidget(currentFile);
    connect(selectBtn, &QPushButton::clicked, this, [=] {
        QString f = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                        openFolder,
                                                        tr("Manga zip Files (*.zip *.cbz)"));
        if (!f.isEmpty()) {
            QFileInfo info(f);
            openFolder = info.absolutePath();
            s->setValue("openFolder", openFolder);
            loadFile(f);
        }
    });
    resize(WIDTH, 800);
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
    int w = qMax(WIDTH, size().width());
    int imgWidth = w -4;
    return imgWidth;
}

void MainWindow::resizeTimeout()
{
    loadPixmaps();
}


void MainWindow::resizeEvent(QResizeEvent *e)
{
    resizeTimer->start();
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
    zip = new QuaZip(f);
    zip->open(QuaZip::Mode::mdUnzip);
    fileList = zip->getFileNameList();
    zip->goToFirstFile();
    int total = zip->getEntriesCount();

    auto msg = "/ " + QString::number(total);
    currentPage->setText(msg);
    currentFile->setText(info.fileName());
    setWindowTitle(QString("zipview - %1").arg(info.fileName()));
    file = new QuaZipFile(zip);
    zip->close();

    for (int i = 1; i <= total; ++i) {
        pageComboBox->addItem(QString::number(i));
    }
    loadFromIndex();
}

void MainWindow::loadFromIndex(int index)
{
    allPixmaps.clear();
    int start = qMax(0, index - 1);
    int end = qMin(start + 6, fileList.count() - 1);
    pageComboBox->setCurrentIndex(start);
    zip->open(QuaZip::Mode::mdUnzip);
    for (int i = start; i <= end; ++i) {
        zip->setCurrentFile(fileList.at(i));
        file->open(QIODevice::ReadOnly);
        auto data = file->readAll();
        QPixmap pix;
        pix.loadFromData(data);
        allPixmaps.append(pix);
        file->close();
        currentIndex = i;
    }
    zip->close();
    while (allPixmaps.count() < 7) {
        allPixmaps.append(QPixmap());
    }
    loadPixmaps();
    if (index == 0) {
        area->verticalScrollBar()->setValue(0);
    }
    else {
        area->ensureWidgetVisible(allLabels.at(1));
    }

}

void MainWindow::loadNext()
{
    if (!(currentIndex + 1 < fileList.count()))
        return;

    currentIndex++;
    auto fileName = fileList.at(currentIndex);
    zip->open(QuaZip::Mode::mdUnzip);
    zip->setCurrentFile(fileName);
    allPixmaps.removeFirst();
    file->open(QIODevice::ReadOnly);
    auto data = file->readAll();
    QPixmap pix;
    pix.loadFromData(data);
    allPixmaps.append(pix);
    file->close();
    zip->close();

    int h = allLabels.first()->contentsRect().height();
    int val = area->verticalScrollBar()->value();
    loadPixmaps();
    area->verticalScrollBar()->setValue(val - h);
}

void MainWindow::loadPrev()
{
    if (currentIndex < 7)
        return;

    currentIndex--;
    int first = currentIndex - 6;
    auto fileName = fileList.at(first);
    zip->open(QuaZip::Mode::mdUnzip);
    zip->setCurrentFile(fileName);
    allPixmaps.removeLast();
    file->open(QIODevice::ReadOnly);
    auto data = file->readAll();
    QPixmap pix;
    pix.loadFromData(data);
    allPixmaps.prepend(pix);
    file->close();
    zip->close();


    int val = area->verticalScrollBar()->value();
    loadPixmaps();
    int h = allLabels.first()->contentsRect().height();
    area->verticalScrollBar()->setValue(val + h - 60);
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
    for (int i = 0; i < allLabels.count(); ++i) {
        QRect lblGeo = allLabels.at(i)->geometry();
        lblGeo.translate(0, wid->geometry().y());
        if (lblGeo.intersects(area->geometry())) {
            page = i + 1;
            break;
        }
    }
    page = currentIndex - qMin(fileList.count(), 7) + page;
    pageComboBox->setCurrentIndex(page);
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
