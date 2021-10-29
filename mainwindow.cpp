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
//    TrackEntry *trackEntry = new TrackEntry();
//    trackEntry->setText("Button text", "Howdy partner");
//    QListWidgetItem *lwi = new QListWidgetItem(ui->trackerListWidget);
//    ui->trackerListWidget->addItem(lwi);
//    ui->trackerListWidget->setItemWidget(lwi, trackEntry);
//    lwi->setSizeHint(QSize(50, 50)); //width ,height
}
