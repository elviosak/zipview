#ifndef MODEL_H
#define MODEL_H

#include <QAbstractItemModel>
#include <QObject>

class Model : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit Model(QString path, QObject *parent = nullptr);
    ~Model() = default;

private:
    QString filePath;


index' in 'Model'
parent' in 'Model'
rowCount' in 'Model'
columnCount' in 'Model'
data' in 'Model'
};

#endif // MODEL_H
