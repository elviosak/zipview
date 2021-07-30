#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
//class Scroll;
class MainWindow : public QMainWindow
{
    Q_OBJECT

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
    QList<QLabel *> allLabels;
    QList<QPixmap> allPixmaps;


    int getImageWidth();
    void resizeTimeout();
    void resizeEvent(QResizeEvent *);
    void loadFile(QString f);
    void loadFive(int index = 0);
    void loadNext();
    void loadPrev();
    void loadPixmaps();
    void scrollChanged(int v);
    void updatePage();
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
public:
    MainWindow(int argc, char *argv[], QWidget *parent = nullptr);
    ~MainWindow();


};
#endif // MAINWINDOW_H
