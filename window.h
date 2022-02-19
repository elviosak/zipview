#ifndef WINDOW_H
#define WINDOW_H

#include "view.h"

#include <QMainWindow>
#include <QWidget>

#include <QToolBar>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

#include <QSettings>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QString>

class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window(QWidget *parent = nullptr);
    ~Window() = default;
private:
    QSettings *s;
    View *view;
    QToolBar *bar;
    QComboBox *combo;
    QLabel *label;
    QStringList fileList;

    //settings
    Qt::ToolBarArea barPosition;
    bool barVisible;


    QString fileName;
    QString currentFile;
    void onFileLoaded(const QString &file);
    void onFileListLoaded(const QStringList &list);
    void onCurrentFileChanged(const QString &file);
    void updateTitle();
    void showMenu(const QPoint &p);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
signals:

};

#endif // WINDOW_H
