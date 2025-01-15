#ifndef LOADER_H
#define LOADER_H

#include <QMap>
#include <QModelIndex>
#include <QObject>
#include <QStandardItem>
#include <QStringList>
#include <QThread>

#include <K7Zip>
// #include <KArchive>
// #include <KZip>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class Loader : public QThread
{
    Q_OBJECT
public:
    Loader(QObject* parent = nullptr);
    ~Loader();
    void load7zip(const QString& path);
    void loadZip(const QString& path);
    void setFile(const QString& path);
    void addToQueue(const QModelIndex& index);
    void run() override;
    QStringList getEntries(KArchive* zip, const KArchiveDirectory* dir);

private:
    QString fileName;
    QuaZip* zip = nullptr;

    K7Zip* sevenZip = nullptr;
    QList<QModelIndex> queue;

signals:
    void pixmapLoaded(const QModelIndex& index, const QPixmap& pix);
    void itemLoaded(QStandardItem* item);
    void fileListLoaded(const QStringList& fileList);
};

#endif // LOADER_H
