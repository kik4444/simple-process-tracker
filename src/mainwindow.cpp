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

    QStandardItemModel* model = new QStandardItemModel();

    model->setHorizontalHeaderItem(0, new QStandardItem("First column"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Second column"));

    model->appendRow(new QStandardItem("1"));
    model->appendRow(new QStandardItem("2"));

    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
