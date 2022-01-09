/*    This file is part of Simple Process Tracker.
 *
 *    Simple Process Tracker is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Simple Process Tracker is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Simple Process Tracker.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../../mainwindow.h"
#include "trackentry.h"
#include "ui_trackentry.h"

TrackEntry::TrackEntry(QWidget *parent) : QWidget(parent), ui(new Ui::TrackEntry)
{
    ui->setupUi(this);

    //Set timer for updating the duration
    updateTimer = new QTimer(this);
    updateTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(updateTimer, &QTimer::timeout, this, &TrackEntry::updateDuration);

    //Set timer for polling the process
    processPollTimer = new QTimer(this);
    processPollTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(processPollTimer, &QTimer::timeout, this, &TrackEntry::pollProcess);

    //Set listening for clicks on the icon label
    ui->iconLabel->installEventFilter(this);

    //Set icon
    ui->iconLabel->setPixmap(QPixmap(iconPath));
    ui->iconLabel->setMaximumSize(iconSize, iconSize);
}

TrackEntry::~TrackEntry()
{
    delete ui;
}

bool TrackEntry::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->iconLabel && event->type() == QEvent::MouseButtonPress)
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.svg *.svgz *.png *.jpg *.jpeg *.webp *.bmp *.tiff *.tif)");

        QPixmap icon(fileName);

        if (!fileName.isEmpty() && !icon.isNull())
        {
            ui->iconLabel->setPixmap(icon);
            iconPath = fileName;
        }
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

QString TrackEntry::getIconPath()
{
    return iconPath;
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

bool TrackEntry::getHidden()
{
    return hidden;
}

void TrackEntry::setTimerState()
{
    if (trackingIsActive && !hidden)
    {
        //Perform check immediately on startup and set the interval later
        processPollTimer->start();
    }
    else
    {
        processPollTimer->stop();
        updateTimer->stop();
    }
}

void TrackEntry::setData(QString processName, QString iconPath, uint processDuration, QString dateAdded, bool trackingIsActive, bool hidden)
{
    ui->hideButton->setEnabled(true);
    this->iconPath = iconPath;
    this->processDuration = processDuration;
    this->trackingIsActive = trackingIsActive;
    this->hidden = hidden;

    ui->selectButton->hide();
    ui->lineEdit->setText(processName);
    ui->iconLabel->setPixmap(QPixmap(iconPath));
    ui->durationButton->setText(parseProcessDuration(processDuration));
    ui->dateAddedLabel->setText(dateAdded);
    ui->trackingCheckBox->setChecked(trackingIsActive);

    setTimerState();
    setHideChanges();
}

void TrackEntry::pollProcess()
{
    processPollTimer->setInterval(MainWindow::userSettings["runningPollInterval"].toUInt() * 1000);

    if (Platform::isProcessRunning(getProcessName(), this))
    {
        if (!updateTimer->isActive())
        {
            updateTimer->start(updateTimerInterval);
            ui->lastSeenLabel->setText("Last seen");
        }
    }
    else
    {
        if (updateTimer->isActive())
            setLastSeenNow();

        processPollTimer->setInterval(MainWindow::userSettings["stoppedPollInterval"].toUInt() * 1000);
        updateTimer->stop();
    }
}

void TrackEntry::updateDuration()
{
    processDuration += updateInterval;
    ui->durationButton->setText(parseProcessDuration(processDuration));
}

void TrackEntry::on_selectButton_clicked()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::processChosen, this, &TrackEntry::processChosen);
    processDialog->exec();
}

void TrackEntry::processChosen(QString processName)
{
    ui->hideButton->setEnabled(true);
    trackingIsActive = true;

    ui->lineEdit->setText(processName);
    ui->dateAddedLabel->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd"));
    ui->selectButton->hide();
    ui->trackingCheckBox->setChecked(trackingIsActive);
    setTimerState();
}

void TrackEntry::on_trackingCheckBox_stateChanged(int arg1)
{
    ui->trackingCheckBox->setChecked(arg1);
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
    QString durationInput = QInputDialog::getText(this, "Custom duration", "Set custom duration",
        QLineEdit::Normal, parseProcessDuration(getProcessDuration()), &ok);

    QStringList splitDuration = durationInput.split(":");
    if (ok && splitDuration.size() == 3)
        processDuration = splitDuration[0].toUInt() * 3600 + splitDuration[1].toUInt() * 60 + splitDuration[2].toUInt();
}

void TrackEntry::on_hideButton_clicked()
{
    hidden = !hidden;
    emit saveProcessData();
    if (hidden)
        emit removeHiddenProcess(getProcessName());

    setTimerState();
    setHideChanges();
}

void TrackEntry::setHideChanges()
{
    ui->hideButton->setText(hidden ? "Unhide" : "Hide");

    //If entry is hidden, disable it entirely except the unhide and remove buttons
    foreach (QWidget* widget, this->findChildren<QWidget*>())
        widget->setEnabled(!hidden);

    ui->hideButton->setEnabled(true);
    ui->removeButton->setEnabled(true);
}

void TrackEntry::updateProcessLastSeenIfRunning()
{
    if (updateTimer->isActive())
        setLastSeenNow();
}

void TrackEntry::setLastSeenNow()
{
    ui->lastSeenLabel->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
}
