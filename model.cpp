#include "model.h"

Model::Model(QString path, QObject *parent)
    : QAbstractItemModel{parent}
    , filePath{path}
{

}
