#include "mainwindow.h"
#include "./ui_mainwindow.h"

QVariantMap MainWindow::userSettings;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Load user settings
    {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");
        this->userSettings["runningPollInterval"] = settings.value("runningPollInterval", 5).toUInt();
        this->userSettings["stoppedPollInterval"] = settings.value("stoppedPollInterval", 10).toUInt();
    }

    //Load process data
    loadProcessData(false);

    //Create system tray icon
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/Assets/Icons/app-icon.svg"));

    QMenu *systemTrayIconMenu = new QMenu();
    systemTrayIconMenu->addAction("Open", this, &MainWindow::trayIconActionOpen);
    systemTrayIconMenu->addAction("Exit", this, &MainWindow::trayIconActionExit);

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    systemTrayIcon->setContextMenu(systemTrayIconMenu);
    systemTrayIcon->show();

    //Set autosave timer
    QTimer *autoSaveTimer = new QTimer(this);
    autoSaveTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::saveProcessData);
    autoSaveTimer->start(autoSaveTimerInterval);
}

MainWindow::~MainWindow()
{
    saveProcessData();
    delete ui;
}

void MainWindow::loadProcessData(bool showHidden)
{
    ui->trackerListWidget->clear();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "processList");
    QMap<uint, QString> processOrder;
    foreach (QString process, settings.childGroups())
    {
        if (!settings.value(process + "/hidden").toBool() || showHidden)
        {
            uint position = settings.value(process + "/position").toUInt();
            while (processOrder.contains(position))
                position++;

            processOrder.insert(position, process);
        }
    }

    foreach (uint position, processOrder.keys())
    {
        settings.beginGroup(processOrder[position]);

        TrackEntry *trackEntry = new TrackEntry();

        trackEntry->setData(processOrder[position], settings.value("iconPath").toString(), settings.value("duration", 0).toUInt(),
            settings.value("dateAdded").toString(), settings.value("trackingIsActive", false).toBool(), settings.value("hidden", false).toBool());

        configureTrackEntry(trackEntry);

        settings.endGroup();
    }
}

void MainWindow::saveProcessData()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "processList");

    for (uint i = 0; i < ui->trackerListWidget->count(); i++)
    {
        QListWidgetItem *widgetItem = ui->trackerListWidget->item(i);
        TrackEntry *trackEntry = dynamic_cast<TrackEntry*>(ui->trackerListWidget->itemWidget(widgetItem));

        QString processName = trackEntry->getProcessName();
        if (!processName.isEmpty())
        {
            settings.beginGroup(processName);

            settings.setValue("iconPath", trackEntry->getIconPath());
            settings.setValue("duration", trackEntry->getProcessDuration());
            settings.setValue("dateAdded", trackEntry->getDateAdded());
            settings.setValue("trackingIsActive", trackEntry->getTrackingIsActive());
            settings.setValue("hidden", trackEntry->getHidden());
            settings.setValue("position", i);

            settings.endGroup();
        }
    }
}

void MainWindow::on_actionAdd_triggered()
{
    TrackEntry *trackEntry = new TrackEntry();
    configureTrackEntry(trackEntry);
}

void MainWindow::configureTrackEntry(TrackEntry *trackEntry)
{
    connect(trackEntry, &TrackEntry::removeClearedEntries, this, &MainWindow::removeClearedEntries);
    connect(this, &MainWindow::forcePollProcesses, trackEntry, &TrackEntry::pollProcess);
    connect(trackEntry, &TrackEntry::saveProcessData, this, &MainWindow::saveProcessData);
    connect(trackEntry, &TrackEntry::removeHiddenProcess, this, &MainWindow::removeHiddenProcess);
    QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->trackerListWidget);
    ui->trackerListWidget->addItem(listWidgetItem);
    ui->trackerListWidget->setItemWidget(listWidgetItem, trackEntry);
    listWidgetItem->setSizeHint(QSize(trackEntrySize, trackEntrySize));
}

void MainWindow::removeClearedEntries()
{
    for (uint i = 0; i < ui->trackerListWidget->count(); i++)
    {
        QListWidgetItem *widgetItem = ui->trackerListWidget->item(i);
        TrackEntry *trackEntry = dynamic_cast<TrackEntry*>(ui->trackerListWidget->itemWidget(widgetItem));
        if (trackEntry->getProcessName().isEmpty())
            delete widgetItem;
    }
}

void MainWindow::removeHiddenProcess(QString processName)
{
    if (!showHidden)
    {
        for (uint i = 0; i < ui->trackerListWidget->count(); i++)
        {
            QListWidgetItem *widgetItem = ui->trackerListWidget->item(i);
            TrackEntry *trackEntry = dynamic_cast<TrackEntry*>(ui->trackerListWidget->itemWidget(widgetItem));
            if (QString::compare(trackEntry->getProcessName(), processName) == 0)
            {
                delete widgetItem;
                break;
            }
        }
    }
}

void MainWindow::trayIconActionOpen()
{
    this->show();
}

void MainWindow::trayIconActivated(int activationReason)
{
    if (activationReason == QSystemTrayIcon::Trigger)
        systemTrayIcon->contextMenu()->popup(QCursor::pos());
}

void MainWindow::trayIconActionExit()
{
    qApp->quit();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionOptions_triggered()
{
    Options *options = new Options();
    options->show();
}

void MainWindow::on_actionPoll_triggered()
{
    emit forcePollProcesses();
}

void MainWindow::on_actionShow_hidden_triggered()
{
    showHidden = !showHidden;
    ui->actionShow_hidden->setText(showHidden ? "Hide hidden" : "Show hidden");
    ui->actionPoll->setEnabled(!showHidden);

    saveProcessData();
    loadProcessData(showHidden);
}
