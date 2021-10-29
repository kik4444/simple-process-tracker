#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName(QLatin1String("Simple Process Tracker"));
    QCoreApplication::setOrganizationName(QLatin1String("Simple Process Tracker"));

    MainWindow w;
    w.show();
    return a.exec();
}
