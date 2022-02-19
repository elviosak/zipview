#include "listmodel.h"

#include <QMessageBox>
#include <QRegularExpression>

ListModel::ListModel(QString path, QObject *parent)
    : QAbstractListModel{parent}
    , filePath{path}
{
    loadFile(path);
}



void ListModel::loadFile(QString f)
{
    QFileInfo info(f);
    if (!info.exists() && !(info.suffix().toLower() == "zip" || info.suffix().toLower() == "cbz")) {
        QMessageBox::warning(nullptr, "Invalid File", QString("file: %1\n is not valid").arg(f));
        return;
    }
    QuaZip zip(f);
    zip.open(QuaZip::Mode::mdUnzip);

    QuaZipFile file(&zip);
    QRegularExpression re("(\\.bmp|\\.png|\\.jpg|\\.jpeg|\\.webp)$", QRegularExpression::PatternOption::CaseInsensitiveOption);
    QStringList fileList = zip.getFileNameList().filter(re);
//    std::sort(fileList.begin(), fileList.end(), compareNumbers);
    int total = fileList.count();
    for (int i = 0; i < fileList.count(); ++i) {
        auto index = createIndex(i, 0);
        setData(index, fileList.at(i), FileName);
        zip.setCurrentFile(fileList.at(i));
        file.open(QuaZipFile::ReadOnly);
        auto img = QImage::fromData(file.readAll());
        file.close();
        QPixmap pix = QPixmap::fromImage(img);
        setData(index, pix, Image);
//        setData(index, pix.size(), Size);
    }

    zip.close();
}
