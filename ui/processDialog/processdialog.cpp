#include "../../mainwindow.h"
#include "processdialog.h"
#include "ui_processdialog.h"

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);

    processList = Platform::getProcessList(this);
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
    emit processChosen(item->text().remove("\n").remove("\r"));
    this->~ProcessDialog();
}

void ProcessDialog::on_buttonBox_accepted()
{
    on_listWidget_itemActivated(ui->listWidget->currentItem());
}

void ProcessDialog::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();

    if (!arg1.isEmpty())
    {
        QStringList results = processList.filter(arg1, Qt::CaseInsensitive);
        ui->listWidget->addItems(results);
    }
    else
        ui->listWidget->addItems(processList);
}

