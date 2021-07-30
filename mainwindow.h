#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

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
    QScrollArea *area;
    QTimer *resizeTimer;
    QComboBox *pageComboBox;
    QLabel *currentPage;
    QLabel *currentFile;
    QPushButton *selectBtn;
    QuaZip *zip;
    QuaZipFile *file;
    QStringList fileList;
    int currentIndex;
    QLabel *defaultLabel;
    QLabel *endLabel;
    QList<QLabel *> allLabels;
    QList<QPixmap> allPixmaps;


    int getImageWidth();
    void resizeTimeout();
    void resizeEvent(QResizeEvent *);
    void loadFile(QString f);
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
