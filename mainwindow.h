#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "delegate.h"

class Area : public QScrollArea {
    Q_OBJECT;
    using QScrollArea::QScrollArea;
    void resizeEvent(QResizeEvent *e) {
        QScrollArea::resizeEvent(e);
        emit resized();
    };
signals:
    void resized();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int argc, char *argv[], QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSettings *s;
    QString openFolder;
    QWidget *wid;
    QVBoxLayout *vbox;
    Area *area;
    QTimer *resizeTimer;
    QComboBox *pageComboBox;
    QLabel *currentPage;
    QPushButton *selectBtn;
    QString zipName;
    QString fileName;
    QStringList fileList;
    int lastIndex;
    QLabel *defaultLabel;
    QLabel *endLabel;
    QListView *thumbView;
    QStandardItemModel *model;
    QList<QLabel *> allLabels;
    QList<QPixmap> allPixmaps;

    int getImageWidth();
    void resizeTimeout();
    void loadFile(QString f);
    void loadThumbs();
    void loadFromIndex(int index = 0);
    void loadNext();
    void loadPrev();
    void loadPixmaps();
    void scrollChanged(int v);
    void updatePage();
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};
#endif // MAINWINDOW_H
