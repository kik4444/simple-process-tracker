#include "../../mainwindow.h"
#include "trackentry.h"
#include "ui_trackentry.h"

TrackEntry::TrackEntry(QWidget *parent) : QWidget(parent), ui(new Ui::TrackEntry)
{
    ui->setupUi(this);
}

QString TrackEntry::parseProcessDuration(uint processDuration)
{
    uint minutes = processDuration / 60;
    uint hours = minutes / 60;
    return QStringLiteral("%1").arg(hours, 2, 10, QLatin1Char('0')) + ":"
            + QStringLiteral("%1").arg(minutes % 60, 2, 10, QLatin1Char('0')) + ":"
            + QStringLiteral("%1").arg(processDuration % 60, 2, 10, QLatin1Char('0'));
}

QString TrackEntry::getProcessName()
{
    return ui->lineEdit->text();
}

void TrackEntry::setData(QString processName, uint processDuration)
{
    ui->selectButton->setEnabled(false);
    ui->lineEdit->setText(processName);
    ui->lineEdit->setReadOnly(true);
    ui->durationLabel->setText(parseProcessDuration(processDuration));
}

void TrackEntry::on_selectButton_clicked()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::processChosen, this, &TrackEntry::processChosen);
    processDialog->exec();
}

void TrackEntry::processChosen(QString processName)
{
    ui->lineEdit->setText(processName);
    ui->selectButton->setEnabled(false);
    ui->lineEdit->setReadOnly(true);
    trackingIsActive = true;
}

TrackEntry::~TrackEntry()
{
    delete ui;
}
