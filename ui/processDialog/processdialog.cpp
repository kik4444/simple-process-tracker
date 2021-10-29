#include "../../mainwindow.h"
#include "processdialog.h"
#include "ui_processdialog.h"

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);

    QProcess *process = new QProcess(this);
    QStringList processList;

    #if defined Q_OS_LINUX

    process->start("ps", QStringList() << "-eo" << "cmd");
    process->waitForFinished();
    processList = QString(process->readAllStandardOutput()).split("\n");

    #elif defined Q_OS_MACOS

    //do macos stuff

    #elif defined Q_OS_WINDOWS

    //do windows stuff

    #endif

    processList.removeDuplicates();
    processList.removeAll(QString(""));
    processList.sort(Qt::CaseInsensitive);
    ui->listWidget->addItems(processList);
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

void ProcessDialog::on_buttonBox_rejected()
{
    this->~ProcessDialog();
}

void ProcessDialog::on_listWidget_itemActivated(QListWidgetItem *item)
{
    emit processChosen(item->text());
    this->~ProcessDialog();
}

void ProcessDialog::on_buttonBox_accepted()
{
    on_listWidget_itemActivated(ui->listWidget->currentItem());
}
