#include "../../mainwindow.h"
#include "trackentry.h"
#include "ui_trackentry.h"

TrackEntry::TrackEntry(QWidget *parent) : QWidget(parent), ui(new Ui::TrackEntry)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, this, &TrackEntry::updateDuration);

    //Set listening for clicks on the icon label
    ui->iconLabel->installEventFilter(this);

    //For default icon
    ui->iconLabel->setPixmap(QPixmap(":/Assets/Icons/app-icon.svg").scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

TrackEntry::~TrackEntry()
{
    delete ui;
}

bool TrackEntry::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->iconLabel && event->type() == QEvent::MouseButtonPress)
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff *.tif)");

        if (!fileName.isEmpty())
            ui->iconLabel->setPixmap(QPixmap(fileName).scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    return false;
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

QString TrackEntry::getDateAdded()
{
    return ui->dateAddedLabel->text();
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

void TrackEntry::setData(QString processName, uint processDuration, QString dateAdded, bool trackingIsActive)
{
    this->processDuration = processDuration;
    this->trackingIsActive = trackingIsActive;

    ui->selectButton->hide();
    ui->lineEdit->setText(processName);
    ui->durationButton->setText(parseProcessDuration(processDuration));
    ui->dateAddedLabel->setText(dateAdded);
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

    #elif defined Q_OS_MACOS

    //do macos stuff

    #elif defined Q_OS_WINDOWS

    //do windows stuff

    #endif

    if (processIsRunning)
    {
        processDuration += updateInterval;
        ui->durationButton->setText(parseProcessDuration(processDuration));
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
    ui->dateAddedLabel->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd"));
    ui->selectButton->hide();
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
        confirmDialog.setText(QString("Are you sure you wish to remove %1?").arg(getProcessName()));
        confirmDialog.setIcon(QMessageBox::Question);
        confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmDialog.setDefaultButton(QMessageBox::No);
        int answer = confirmDialog.exec();

        if (answer != QMessageBox::Yes)
            return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "processList");
    settings.remove(getProcessName());
    ui->lineEdit->clear();
    emit removeClearedEntries();
}


void TrackEntry::on_durationButton_clicked()
{
    bool ok;
    QString durationInput = QInputDialog::getText(this, "Custom duration", "Set custom duration", QLineEdit::Normal, "00:00:00", &ok);
    QStringList splitDuration = durationInput.split(":");
    if (ok && splitDuration.size() == 3)
        processDuration = splitDuration[0].toUInt() * 3600 + splitDuration[1].toUInt() * 60 + splitDuration[2].toUInt();
}
