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
    qDebug() << "Add new action";
    TrackEntry *trackEntry = new TrackEntry();
    trackEntry->setData("Some process", 100);
    QListWidgetItem *LWI = new QListWidgetItem(ui->trackerListWidget);
    ui->trackerListWidget->addItem(LWI);
    ui->trackerListWidget->setItemWidget(LWI, trackEntry);
    LWI->setSizeHint(QSize(50, 50)); //width ,height
}
