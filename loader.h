#ifndef LOADER_H
#define LOADER_H

#include <QThread>
#include <QObject>
#include <QModelIndex>
#include <QStandardItem>
#include <QMap>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class Loader : public QThread
{
    Q_OBJECT
public:
    Loader(QObject *parent = nullptr);
    ~Loader();
    void setFile(QString path);
    void addToQueue(const QModelIndex &index);
    void run() override;
private:
    QString fileName;
    QuaZip *zip = nullptr;
    QList<QModelIndex> queue;
signals:
    void pixmapLoaded(const QModelIndex &index, const QPixmap &pix);
    void itemLoaded(QStandardItem *item);
    void fileListLoaded(const QStringList &fileList);
};

#endif // LOADER_H
