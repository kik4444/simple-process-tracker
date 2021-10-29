#include "../../mainwindow.h"
#include "processdialog.h"
#include "ui_processdialog.h"

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);

    QProcess *process = new QProcess(this);

    #if defined Q_OS_LINUX

    //do linux stuff
    //ps -eo cmd
    process->start("ps", QStringList() << "-eo" << "cmd");
    process->waitForFinished();
    QStringList processList = QString(process->readAllStandardOutput()).split("\n") << "";
    foreach (QString p, processList)
    {
        qDebug() << p;
    }

    #elif defined Q_OS_MACOS

    //do macos stuff

    #elif defined Q_OS_WINDOWS

    //do windows stuff

    #endif
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}
