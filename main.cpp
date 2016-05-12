#include "mainwindow.h"
#include <QApplication>
#include "QString"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    MainWindow w(a.applicationDirPath());    
    if(!QString(argv[1]).contains("--tray"))
        w.show();

    return a.exec();
}
