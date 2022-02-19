#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ImageData {
        FileName = Qt::UserRole + 1,
        Image,
        Size
    };
    Q_ENUM(ImageData);
    explicit ListModel(QString path, QObject *parent = nullptr);

private:
    QString filePath;
    QList<QModelIndex> indexes;

    void loadFile(QString f);


    Qt::ItemFlags flags(const QModelIndex &index) const {
        return index.flags();
    };
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
        return index.data(role);
    };
    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return indexes.count();
    };
};

#endif // LISTMODEL_H
