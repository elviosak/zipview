#include "loader.h"
#include <QBuffer>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QRegularExpression>

bool compareNumbers(const QString& v1, const QString& v2)
{

    static QRegularExpression r("(\\D+|\\d+)");
    auto match1 = r.globalMatch(v1);
    auto match2 = r.globalMatch(v2);
    bool ok1, ok2;
    while (match1.hasNext() && match2.hasNext()) {
        QString str1 = match1.next().captured(1);
        QString str2 = match2.next().captured(1);
        if (str1 == str2) {
            continue;
        }

        int num1 = str1.toInt(&ok1);
        int num2 = str2.toInt(&ok2);
        if (ok1 && ok2 && (num1 != num2)) {
            return (num1 < num2);
        }
        else {
            return (str1 < str2);
        }
    }
    return v1 < v2;
}

Loader::Loader(QObject* parent)
    : QThread { parent }
{
    connect(this, &QThread::finished, this, [=] {
        if (queue.count() > 0) {
            start();
        }
    });
}
Loader::~Loader()
{
    if (nullptr != zip) {
        zip->close();
        delete zip;
    }
}

QStringList Loader::getEntries(KArchive* zip, const KArchiveDirectory* dir)
{
    QStringList list;
    auto entries = dir->entries();
    auto base = dir->name();
    if (base == "/") {
        base = "";
    }
    else {
        base += "/";
    }
    for (int i = 0; i < entries.size(); ++i) {
        auto fName = entries[i];
        const KArchiveEntry* entry = dir->entry(fName);
        if (entry->isDirectory()) {
            KArchiveDirectory* newDir = (KArchiveDirectory*)entry;
            list.append(getEntries(zip, newDir));
        }
        else {
            auto fullPath = QString("%1%2").arg(base).arg(entry->name());
            list.append(fullPath);
            // qDebug() << "getEntry: " << fName << "isDir: " << entry->isDirectory() << "fullPath: " << fullPath;
        }
    }

    return list;
}

// Using KArchive
void Loader::setFile(QString path)
{
    if (nullptr != zip) {
        zip->close();
        delete zip;
    }
    fileName = path;
    QFileInfo info(path);
    if (!info.exists()) {
        return;
    }
    if (info.suffix().toLower() == "zip" || info.suffix().toLower() == "cbz") {
        zip = new KZip(path);
    }
    else if (info.suffix().toLower() == "7z") {
        zip = new K7Zip(path);
    }
    else {
        return;
    }
    zip->open(QIODeviceBase::ReadOnly);

    auto dir = zip->directory();
    QStringList tempList = getEntries(zip, dir);

    std::sort(tempList.begin(), tempList.end(), compareNumbers);
    QStringList fileList;

    // This loop populates the model with items and stores the filename (to load the image later)
    // and size (to keep the scrollbar the same size and in the same position while loading images)
    // the images are loaded on demand on Loader::run().
    for (int i = 0; i < tempList.size(); ++i) {
        auto fName = tempList[i];
        auto file = dir->file(fName);
        if (nullptr != file) {
            auto item = new QStandardItem;
            item->setText(fName);
            // auto img = QImage::fromData(file->data());
            auto device = file->createDevice();
            QImageReader reader(device);
            if (reader.canRead()) {
                QSize s = reader.size();
                item->setData(fName, Qt::DisplayRole);
                item->setData(s, Qt::SizeHintRole);
                fileList.append(fName);
                // qDebug() << "file " << fName;
                emit itemLoaded(item);
            }
            device->close();
            device->deleteLater();
        }
    }
    emit fileListLoaded(fileList);
}

// Using Quazip
// void Loader::setFile(QString path)
// {
//     if (nullptr != zip) {
//         zip->close();
//         zip->~QuaZip();
//     }

//     fileName = path;
//     zip = new QuaZip(fileName);
//     zip->open(QuaZip::mdUnzip);

//     //    QRegularExpression re("(\\.bmp|\\.png|\\.jpg|\\.jpeg|\\.webp)$", QRegularExpression::PatternOption::CaseInsensitiveOption);

//     QStringList fileList;
//     QStringList tempList = zip->getFileNameList(); //.filter(re);

//     std::sort(tempList.begin(), tempList.end(), compareNumbers);

//     QuaZipFile file(zip);
//     while (tempList.size() > 0) {
//         auto fName = tempList.takeFirst();
//         auto item = new QStandardItem;
//         item->setText(fName);
//         zip->setCurrentFile(fName);
//         file.open(QuaZipFile::ReadOnly);
//         QSize s;
//         bool canRead = false;
//         if (true) {
//             QImageReader reader(&file);
//             s = reader.size();
//             //            reader.
//             canRead = reader.canRead();
//         }

//         if (canRead) {
//             qDebug() << "size1" << s;
//             if (!s.isValid()) {
//                 QImage img = QImage::fromData(file.readAll());
//                 s = img.size();
//                 qDebug() << "size2" << s;
//             }
//             item->setData(s, Qt::SizeHintRole);
//             fileList.append(fName);
//             emit itemLoaded(item);
//         }
//         qDebug() << "c1";
//         file.close();
//         qDebug() << "c2";
//     }
//     emit fileListLoaded(fileList);
//     zip->close();
// }

void Loader::addToQueue(const QModelIndex& index)
{
    if (!queue.contains(index)) {
        if (queue.size() > 5) {
            queue.prepend(index);
        }
        else {
            queue.append(index);
        }
    }

    start();
}

void Loader::run()
{
    if (nullptr == zip || queue.count() == 0) {
        return;
    }

    // zip->open(QuaZip::mdUnzip);
    // QuaZipFile file(zip);
    while (queue.size() > 0) {
        auto index = queue.takeFirst();
        QString fName(index.data(Qt::DisplayRole).toString());
        auto file = zip->directory()->file(fName);
        if (nullptr != file) {
            auto pix = QPixmap::fromImage(QImage::fromData(file->data()));
            emit pixmapLoaded(index, pix);
        }
    }
    // zip->close();
}
