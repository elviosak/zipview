#include "window.h"
//#include "view.h"
//#include "listmodel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    w.show();
//    View v;
//    v.show();
//    MainWindow w(argc, argv);
//    w.show();
    return a.exec();
}
