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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "processdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableView->setItemDelegateForColumn(0, new IconDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList() << "Icon" << "Name" << "Duration" << "Date added" << "Last seen");
    ui->tableView->setModel(processTableViewModel);
    //TODO remove after manually saving column widths
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    processTableViewModel->setItem(0, 1, new QStandardItem(QString("test 01")));
//    processTableViewModel->setItem(1, 2, new QStandardItem(QString("test 12")));

//    QStandardItem *item = new QStandardItem();
//    item->setData(QIcon(":/app-icon.svg"), Qt::DecorationRole);
//    processTableViewModel->setItem(0, 0, item);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdd_triggered()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::processChosen, this, &MainWindow::processChosen);
    processDialog->exec();
}

void MainWindow::processChosen(QString processName, QString iconPath)
{

}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    qDebug() << index.column() << " " <<  index.row();
}
