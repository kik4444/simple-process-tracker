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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    processTableViewModel->setHorizontalHeaderLabels(QStringList() << "Icon" << "Name" << "Duration" << "Date added" << "Last seen");

//    for (int row = 0; row < model->rowCount(); ++row) {
//        for (int column = 0; column < model->columnCount(); ++column) {
//            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
//            model->setItem(row, column, item);
//        }
//    }

    ui->tableView->setModel(processTableViewModel);
    processTableViewModel->setItem(0, 1, new QStandardItem(QString("test 01")));
    processTableViewModel->setItem(1, 2, new QStandardItem(QString("test 12")));
}

MainWindow::~MainWindow()
{
    delete ui;
}
