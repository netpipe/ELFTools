#include "qinstall.h"
#include <QApplication>
#include <QFile>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QInstall w;
    QPixmap m( "./Resource/logo.png");

QSplashScreen splash(m);
splash.show();

splash.finish(&w);

    w.show();




    QFile file("Resource/themes/qdarkstyle/qdarkstyle.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    w.setStyleSheet(styleSheet);

    return a.exec();
}
