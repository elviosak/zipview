#include "loader.h"
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QRegularExpression>
#include <QBuffer>

bool compareNumbers(const QString &v1, const QString &v2) {


    static QRegularExpression r("(\\D+|\\d+)");
    auto match1 = r.globalMatch(v1);
    auto match2 = r.globalMatch(v2);
    bool ok1, ok2;
    while (match1.hasNext() && match2.hasNext()) {
        QString str1 = match1.next().captured(1);
        QString str2 = match2.next().captured(1);
        if (str1 == str2)
            continue;

        int num1 = str1.toInt(&ok1);
        int num2 = str2.toInt(&ok2);
        if (ok1 && ok2 && (num1 != num2)) {
            return (num1 < num2);
        }
        else
            return (str1 < str2);
    }
    return v1 < v2;
}

Loader::Loader(QObject *parent)
    : QThread{parent}
{
    connect(this, &QThread::finished, this, [=] {
        if (queue.count() > 0)
            start();
    });
}
Loader::~Loader()
{
    zip->close();
    delete zip;
}

void Loader::setFile(QString path)
{
    if (nullptr != zip) {
        zip->close();
        zip->~QuaZip();
    }

    fileName = path;
    zip = new QuaZip(fileName);
    zip->open(QuaZip::mdUnzip);

//    QRegularExpression re("(\\.bmp|\\.png|\\.jpg|\\.jpeg|\\.webp)$", QRegularExpression::PatternOption::CaseInsensitiveOption);
    QStringList fileList;
    QStringList tempList = zip->getFileNameList();//.filter(re);

    std::sort(tempList.begin(), tempList.end(), compareNumbers);

    QuaZipFile file(zip);
    while (tempList.size() > 0) {
        auto fName = tempList.takeFirst();
        auto item = new QStandardItem;
        item->setText(fName);
        zip->setCurrentFile(fName);
        file.open(QuaZipFile::ReadOnly);
        QSize s;
        bool canRead = false;
        if (true) {
            QImageReader reader(&file);
            s = reader.size();
            canRead = reader.canRead();
        }
//        reader.setAutoDetectImageFormat(false);

//        reader.setFormat("JPG");
//        reader.setDecideFormatFromContent(true);

        if (canRead) {
//            s = reader.size();
//            reader.~QImageReader();
            qDebug() << "size1" << s;
            if (!s.isValid()) {
                file.close();
                file.open(QuaZipFile::ReadOnly);
                QImage img = QImage::fromData(file.readAll());
//                if (reader.read(&img))
//                    qDebug() << "read ok";
//                else
//                    qDebug() << "read failed" << reader.error() << reader.errorString();
    //            QPixmap p;
    //            p.loadFromData(file.readAll());
                s = img.size();
                qDebug() << "size2" << s;
            }

                item->setData(s, Qt::SizeHintRole);
                fileList.append(fName);
                emit itemLoaded(item);
        }
        qDebug() << "c1";
        file.close();
        qDebug() << "c2";
    }
    emit fileListLoaded(fileList);
    zip->close();
}

void Loader::addToQueue(const QModelIndex &index)
{
    if (!queue.contains(index)) {
        if (queue.size() > 5)
            queue.prepend(index);
        else
            queue.append(index);
    }

    start();
}

void Loader::run()
{
    if (nullptr == zip || queue.count() == 0)
        return;

    zip->open(QuaZip::mdUnzip);
    QuaZipFile file(zip);
    while (queue.size() > 0) {
        auto index = queue.takeFirst();
        QString fileName(index.data(Qt::DisplayRole).toString());
        zip->setCurrentFile(fileName);
        file.open(QuaZipFile::ReadOnly);
        auto pix = QPixmap::fromImage(QImage::fromData(file.readAll()));
        file.close();
        emit pixmapLoaded(index, pix);
    }
    zip->close();
}
