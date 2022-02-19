#ifndef VIEW_H
#define VIEW_H

#include "loader.h"
#include "delegate.h"

#include <QListView>
#include <QStandardItemModel>
#include <QTimer>
#include <QMouseEvent>

class View : public QListView
{
    Q_OBJECT
public:
    View(QWidget *parent = nullptr);
    ~View() = default;

    void loadFile(const QString &f);
    void setCurrentFile(int i);

private:
    Loader *loader = nullptr;
    Delegate *delegate = nullptr;
    QStandardItemModel * itemModel = nullptr;
    QTimer *resizeTimer;
    QTimer *unloadTimer;
    QTimer *scrollTimer;
    int toScroll = 0;

    QList<QModelIndex> loadedIndexes;
    int currentScroll = 0;
    int maxScroll = 0;

    void onFileListLoaded(const QStringList &fileList);
    void onItemLoaded(QStandardItem *item);
    void resizeTimeout();
    void unloadTimeout();
    void setItemPixmap(const QModelIndex &index, const QPixmap &pix);
    void scrolled(int value);
signals:
    void fileLoaded(const QString &file);
    void fileListLoaded(const QStringList &fileList);
    void currentFileChanged(const QString &currentFile);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};
#endif // VIEW_H
