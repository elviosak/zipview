#include "window.h"

#include <QMenu>
#include <QPushButton>
#include <QApplication>
#include <QActionGroup>
#include <QDebug>

Window::Window(QWidget *parent)
    : QMainWindow{parent}
{
    setWindowIcon(QIcon(":/zipview"));
    setWindowTitle("zipview");
    setAcceptDrops(true);
    s = new QSettings("zipview", "zipview");
    auto geo = s->value("geometry", QRect()).value<QRect>();
    barPosition = s->value("barPosition", Qt::ToolBarArea::BottomToolBarArea).value<Qt::ToolBarArea>();
    barVisible = s->value("barVisible", true).toBool();

    view = new View;
    setCentralWidget(view);
    connect(view, &View::fileLoaded, this, &Window::onFileLoaded);
    connect(view, &View::fileListLoaded, this, &Window::onFileListLoaded);
    connect(view, &View::currentFileChanged, this, &Window::onCurrentFileChanged);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QMainWindow::customContextMenuRequested, this, &Window::showMenu);

    bar = new QToolBar;
    bar->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
    auto wid = new QWidget;
    wid->setContentsMargins(0,0,0,0);
    auto box = new QHBoxLayout(wid);
    box->setContentsMargins(0,0,0,0);
    combo = new QComboBox;
    label = new QLabel;
    auto btn = new QPushButton("About");
    connect(btn, &QPushButton::clicked, qApp, &QApplication::aboutQt);
    box->addWidget(combo, 1);
    connect(combo, QOverload<int>::of(&QComboBox::activated), view, &View::setCurrentFile);

    box->addWidget(label);
    box->addWidget(btn);

    bar->addWidget(wid);

    addToolBar(barPosition, bar);
    bar->setVisible(barVisible);
    if (!geo.isNull())
        setGeometry(geo);
}

void Window::onFileLoaded(const QString &file)
{
    QUrl url(file);
    fileName = url.fileName();
    updateTitle();
}

void Window::onFileListLoaded(const QStringList &list)
{
    fileList = list;
    updateTitle();
    combo->clear();
    combo->addItems(list);
}

void Window::onCurrentFileChanged(const QString &file)
{
    currentFile = file;
    updateTitle();
    if (combo->findText(file) != -1)
        combo->setCurrentText(file);
}

void Window::updateTitle()
{
    QString title;
    if (fileList.count() > 0) {
        if (currentFile.isEmpty())
            title += QString("(%1) ").arg(fileList.count());
        else
            title += QString("(%1/%2) ")
                    .arg(fileList.indexOf(currentFile) + 1)
                    .arg(fileList.count());
    }
    if (!fileName.isEmpty())
        title += fileName;
    title += " - zipview";
    setWindowTitle(title);
}

void Window::showMenu(const QPoint &p)
{
    QMenu menu(this);
    QMenu *tbMenu = menu.addMenu("ToolBar");
    QAction *a;
    QActionGroup *group = new QActionGroup(tbMenu);
    bool checked;
    a = tbMenu->addAction("Hidden");
    a->setCheckable(true);
    group->addAction(a);
    checked = bar->isHidden();
    a->setChecked(checked);
    connect(a, &QAction::triggered, this, [=] (bool check) {
        if (check)
            bar->hide();
    });

    a = tbMenu->addAction("Top");
    a->setCheckable(true);
    group->addAction(a);
    checked = bar->isVisible() && (toolBarArea(bar) == Qt::TopToolBarArea);
    a->setChecked(checked);
    connect(a, &QAction::triggered, this, [=] (bool check) {
        if (check) {
            addToolBar(Qt::TopToolBarArea, bar);
            bar->show();
        }
    });

    a = tbMenu->addAction("Bottom");
    a->setCheckable(true);
    group->addAction(a);
    checked = bar->isVisible() && (toolBarArea(bar) == Qt::BottomToolBarArea);
    a->setChecked(checked);
    connect(a, &QAction::triggered, this, [=] (bool check) {
        if (check) {
            addToolBar(Qt::BottomToolBarArea, bar);
            bar->show();
        }
    });
//    menu.popup(p);
    menu.exec(mapToGlobal(p));
}

void Window::dragEnterEvent(QDragEnterEvent *e)
{
//    QString fileName;
//    if (e->mimeData()->hasUrls())
//        fileName = e->mimeData()->urls().first().toLocalFile();
//    else if (e->mimeData()->hasText())
//        fileName = e->mimeData()->text().trimmed();
////    qDebug() << "DragEnter" << fileName;
//    if (!fileName.isEmpty())
    if (e->mimeData()->hasUrls() || e->mimeData()->hasText())
        e->accept();
}

void Window::dropEvent(QDropEvent *e)
{
    QString fileName;
    if (e->mimeData()->hasUrls())
        fileName = e->mimeData()->urls().first().toLocalFile();
    else if (e->mimeData()->hasText())
        fileName = e->mimeData()->text().trimmed();
//    qDebug() << "dropEvent" << fileName;
    if (!fileName.isEmpty())
        view->loadFile(fileName);
}

void Window::closeEvent(QCloseEvent *e)
{
    s->setValue("geometry", QVariant::fromValue(geometry()));
    QMainWindow::closeEvent(e);
}
