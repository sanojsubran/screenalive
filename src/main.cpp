#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFile file("style.qss");
    if( file.open(QIODevice::ReadOnly | QIODevice::Text ) ) {
        a.setStyleSheet( file.readAll() );
        file.close();
    }
    w.hide();
    return a.exec();
}
