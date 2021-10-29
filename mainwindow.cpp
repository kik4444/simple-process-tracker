#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");
    foreach (QString process, settings.childGroups())
    {
        settings.beginGroup(process);

        TrackEntry *trackEntry = new TrackEntry();
        trackEntry->setData(process, settings.value("duration", 0).toUInt(), settings.value("trackingIsActive", false).toBool());
        connect(trackEntry, &TrackEntry::removeClearedEntries, this, &MainWindow::removeClearedEntries);
        QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->trackerListWidget);
        ui->trackerListWidget->addItem(listWidgetItem);
        ui->trackerListWidget->setItemWidget(listWidgetItem, trackEntry);
        listWidgetItem->setSizeHint(QSize(trackEntrySize, trackEntrySize)); //width ,height

        settings.endGroup();
    }
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
