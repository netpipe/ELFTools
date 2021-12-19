#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
 //   qDebug() << PRO_PWD;
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("SSL Client");
    w.show();
    return a.exec();
}
