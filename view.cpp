#include "view.h"
//#include "delegate.h"

#include <QMessageBox>
#include <QScrollBar>

#include <QDebug>

View::View(QWidget *parent)
    : QListView(parent)
{
    setDragDropMode(QListView::NoDragDrop);
    setSelectionMode(QListView::NoSelection);
    setResizeMode(QListView::ResizeMode::Adjust);
    setUniformItemSizes(false);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);

    verticalScrollBar()->setSingleStep(50);
    delegate = new Delegate;

    setItemDelegate(delegate);

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &View::scrolled);

    resizeTimer = new QTimer;
    resizeTimer->setInterval(100);
    resizeTimer->setSingleShot(true);
    connect(resizeTimer, &QTimer::timeout, this, &View::resizeTimeout);

    unloadTimer = new QTimer;
    unloadTimer->setInterval(1000);
    connect(unloadTimer, &QTimer::timeout, this, &View::unloadTimeout);
}

void View::setItemPixmap(const QModelIndex &index, const QPixmap &pix)
{
    itemModel->setData(index, pix, Qt::DecorationRole);
    loadedIndexes.append(index);
    update(index);
}

void View::onFileListLoaded(const QStringList &fileList)
{
    emit fileListLoaded(fileList);
}

void View::onItemLoaded(QStandardItem *item)
{
    itemModel->appendRow(item);
}

void View::resizeTimeout()
{
    if (nullptr == itemModel)
        return;

    auto item = new QStandardItem("Refresh");
    itemModel->insertRow(0, item);
    itemModel->removeRow(0);
    if (maxScroll == 0)
        return;
    double fraction = currentScroll / maxScroll;
    maxScroll = verticalScrollBar()->maximum();
    currentScroll = fraction * maxScroll;
    verticalScrollBar()->setValue(currentScroll);
}

void View::unloadTimeout()
{
    auto indexesCopy = loadedIndexes;
    QMutableListIterator<QModelIndex> i(indexesCopy);
    while (i.hasNext()) {
        auto index = i.next();
        if (!index.isValid()) {
            i.remove();
            continue;
        }
        auto rect = visualRect(index);
        if (!rect.isValid() || (rect.y() + rect.height() + height() < 0) || (rect.y() > rect.height() + height())) {
            i.remove();
            itemModel->setData(index, QPixmap(), Qt::DecorationRole);
        }
    }
    loadedIndexes = indexesCopy;
}

void View::loadFile(const QString &f)
{
    unloadTimer->stop();
    QFileInfo info(f);
    if (!info.exists() && !(info.suffix().toLower() == "zip" || info.suffix().toLower() == "cbz")) {
        QMessageBox::warning(this, "Invalid File", QString("file: %1\n is not valid").arg(f));
        return;
    }

    if (nullptr != itemModel)
        itemModel->deleteLater();

    itemModel = new QStandardItemModel;
    setModel(itemModel);
    verticalScrollBar()->setValue(0);

    if (nullptr != loader) {
        disconnect(delegate, &Delegate::requestImage, loader, &Loader::addToQueue);
        disconnect(loader, &Loader::fileListLoaded, this, &View::onFileListLoaded);
        disconnect(loader, &Loader::itemLoaded, this, &View::onItemLoaded);
        disconnect(loader, &Loader::pixmapLoaded, this, &View::setItemPixmap);
        loader->quit();
        loader->deleteLater();
    }
    loader = new Loader;
    connect(delegate, &Delegate::requestImage, loader, &Loader::addToQueue);
    connect(loader, &Loader::fileListLoaded, this, &View::onFileListLoaded);
    connect(loader, &Loader::itemLoaded, this, &View::onItemLoaded);
    connect(loader, &Loader::pixmapLoaded, this, &View::setItemPixmap);
    loader->setFile(f);
    emit fileLoaded(f);
    unloadTimer->start();
}

void View::setCurrentFile(int i)
{
    auto index = itemModel->index(i, 0);
    if (index.isValid())
        scrollTo(index);
}

void View::scrolled(int value)
{
    auto index = indexAt(QPoint(width()/2, height()/2));
    if (!index.isValid())
        return;
    emit currentFileChanged(index.data(Qt::DisplayRole).toString());

    QModelIndex next;
    // down
    if (value > currentScroll) {
        next = itemModel->index(index.row() + 1, 0);
    }
    // up
    else {
        next = itemModel->index(index.row() - 1, 0);
    }

    if (next.isValid()
            && next.data(Qt::DecorationRole).value<QPixmap>().isNull()
            && !next.data(Qt::DisplayRole).toString().isEmpty()) {
        loader->addToQueue(next);
    }
    currentScroll = value;
}

void View::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void View::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}

void View::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);
    resizeTimer->start();
}


