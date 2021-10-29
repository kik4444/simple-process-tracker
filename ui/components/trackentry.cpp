#include "../../mainwindow.h"
#include "trackentry.h"
#include "ui_trackentry.h"

TrackEntry::TrackEntry(QWidget *parent) : QWidget(parent), ui(new Ui::TrackEntry)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, this, &TrackEntry::updateDuration);
}

TrackEntry::~TrackEntry()
{
    delete ui;
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

uint TrackEntry::getProcessDuration()
{
    return this->processDuration;
}

bool TrackEntry::getTrackingIsActive()
{
    return trackingIsActive;
}

void TrackEntry::setTimerState()
{
    if (trackingIsActive)
        timer->start(timerInterval);
    else
        timer->stop();
}

void TrackEntry::setData(QString processName, uint processDuration, bool trackingIsActive)
{
    this->processDuration = processDuration;
    this->trackingIsActive = trackingIsActive;

    ui->selectButton->setEnabled(false);
    ui->lineEdit->setText(processName);
    ui->durationLabel->setText(parseProcessDuration(processDuration));
    ui->trackingCheckBox->setChecked(trackingIsActive);

    setTimerState();
}

void TrackEntry::updateDuration()
{
    bool processIsRunning = false;
    QProcess *process = new QProcess(this);

    #if defined Q_OS_LINUX

    process->start("pgrep", QStringList() << "-nx" << getProcessName());
    process->waitForFinished();
    processIsRunning = !QString(process->readAllStandardOutput()).isEmpty();

    #endif

    if (processIsRunning)
    {
        processDuration += updateInterval;
        ui->durationLabel->setText(parseProcessDuration(processDuration));
    }

    process->deleteLater();
}

void TrackEntry::on_selectButton_clicked()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::processChosen, this, &TrackEntry::processChosen);
    processDialog->exec();
}

void TrackEntry::processChosen(QString processName)
{
    trackingIsActive = true;

    ui->lineEdit->setText(processName);
    ui->selectButton->setEnabled(false);
    ui->trackingCheckBox->setChecked(trackingIsActive);
    setTimerState();
}

void TrackEntry::on_trackingCheckBox_stateChanged(int arg1)
{
    trackingIsActive = (bool)arg1;
    setTimerState();
}

void TrackEntry::on_removeButton_clicked()
{
    if (!getProcessName().isEmpty())
    {
        QMessageBox confirmDialog(this);
        confirmDialog.setWindowTitle("Confirm removal");
        confirmDialog.setText("Are you sure you wish to remove this process?");
        confirmDialog.setIcon(QMessageBox::Question);
        confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmDialog.setDefaultButton(QMessageBox::No);
        int answer = confirmDialog.exec();

        if (answer != QMessageBox::Yes)
            return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");
    settings.remove(getProcessName());
    ui->lineEdit->clear();
    emit removeClearedEntries();
}

