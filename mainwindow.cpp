#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");
    QMap<uint, QString> processOrder;
    foreach (QString process, settings.childGroups())
        processOrder.insert(settings.value(process + "/position").toUInt(), process);

    for (uint i = 0; i < processOrder.size(); i++)
    {
        settings.beginGroup(processOrder[i]);

        TrackEntry *trackEntry = new TrackEntry();
        trackEntry->setData(processOrder[i], settings.value("duration", 0).toUInt(), settings.value("trackingIsActive", false).toBool());
        connect(trackEntry, &TrackEntry::removeClearedEntries, this, &MainWindow::removeClearedEntries);
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->trackerListWidget);
        ui->trackerListWidget->addItem(listWidgetItem);
        ui->trackerListWidget->setItemWidget(listWidgetItem, trackEntry);
        listWidgetItem->setSizeHint(QSize(trackEntrySize, trackEntrySize)); //width ,height

        settings.endGroup();
    }

    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/Assets/Icons/app-icon.svg"));

    QMenu *systemTrayIconMenu = new QMenu();
    systemTrayIconMenu->addAction("Open", this, &MainWindow::trayIconActionOpen);
    systemTrayIconMenu->addAction("Exit", this, &MainWindow::trayIconActionExit);

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    systemTrayIcon->setContextMenu(systemTrayIconMenu);
    systemTrayIcon->show();
}

MainWindow::~MainWindow()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");

    for (uint i = 0; i < ui->trackerListWidget->count(); i++)
    {
        QListWidgetItem *widgetItem = ui->trackerListWidget->item(i);
        TrackEntry *trackEntry= dynamic_cast<TrackEntry*>(ui->trackerListWidget->itemWidget(widgetItem));

        QString processName = trackEntry->getProcessName();
        if (!processName.isEmpty())
        {
            settings.beginGroup(processName);

            settings.setValue("duration", trackEntry->getProcessDuration());
            settings.setValue("trackingIsActive", trackEntry->getTrackingIsActive());
            settings.setValue("position", i);

            settings.endGroup();
        }
    }

    delete ui;
}

void MainWindow::on_actionDebug_triggered()
{

}

void MainWindow::on_actionAdd_triggered()
{
    TrackEntry *trackEntry = new TrackEntry();
    connect(trackEntry, &TrackEntry::removeClearedEntries, this, &MainWindow::removeClearedEntries);
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
        TrackEntry *trackEntry= dynamic_cast<TrackEntry*>(ui->trackerListWidget->itemWidget(widgetItem));
        if (trackEntry->getProcessName().isEmpty())
            delete widgetItem;
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
