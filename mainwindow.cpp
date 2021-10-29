#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdd_triggered()
{
//    TrackEntry *trackEntry = new TrackEntry();
//    trackEntry->setData("Some process", 123456);
//    QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->trackerListWidget);
//    ui->trackerListWidget->addItem(listWidgetItem);
//    ui->trackerListWidget->setItemWidget(listWidgetItem, trackEntry);
//    listWidgetItem->setSizeHint(QSize(50, 50)); //width ,height

    TrackEntry *trackEntry = new TrackEntry();
    QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->trackerListWidget);
    ui->trackerListWidget->addItem(listWidgetItem);
    ui->trackerListWidget->setItemWidget(listWidgetItem, trackEntry);
    listWidgetItem->setSizeHint(QSize(trackEntrySize, trackEntrySize)); //width ,height
}
