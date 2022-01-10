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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ProcessScanner *processScanner = new ProcessScanner();
    connect(this, &MainWindow::checkRunningProcesses, processScanner, &ProcessScanner::checkRunningProcesses);
    connect(processScanner, &ProcessScanner::foundRunningProcess, this, &MainWindow::foundRunningProcess);

    ui->tableView->setItemDelegate(new MyItemDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList() << "Tracking" << "Icon" << "Name" << "Notes" << "Duration" << "Last seen" << "Date added");
    ui->tableView->setModel(processTableViewModel);
    //TODO remove after manually saving column widths
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QTimer *runningProcessDurationsUpdateTimer = new QTimer(this);
    runningProcessDurationsUpdateTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(runningProcessDurationsUpdateTimer, &QTimer::timeout, this, &MainWindow::updateRunningProcessDurations);
    runningProcessDurationsUpdateTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionDebug_triggered()
{
    QMap<QString, int> processList;

    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        processList.insert(processTableViewModel->item(row, ProcessColumns::Name)->text(), row);
    }

    emit checkRunningProcesses(processList);

}

void MainWindow::on_actionAdd_triggered()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::newProcessAdded, this, &MainWindow::newProcessAdded);
    processDialog->exec();
}

void MainWindow::updateRunningProcessDurations()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        QString processName = processTableViewModel->item(row, ProcessColumns::Name)->text();
        if (QString::compare(processTableViewModel->item(row, ProcessColumns::Tracking)->text(), processIsActiveSymbol) == 0
            && runningProcesses.contains(processName))
        {
            processDurations[processName]++;
            processTableViewModel->setItem(row, ProcessColumns::Duration, new QStandardItem(Parser::parseDurationToString(processDurations[processName])));
        }
    }
}

void MainWindow::newProcessAdded(QString processName, QString iconPath)
{
    int newestRow = processTableViewModel->rowCount();
    processTableViewModel->setItem(newestRow, ProcessColumns::Tracking, new QStandardItem(processIsActiveSymbol));
    processTableViewModel->setItem(newestRow, ProcessColumns::Icon, new QStandardItem(QIcon(iconPath.isEmpty() ? ":/app-icon.svg" : iconPath), ""));
    processTableViewModel->setItem(newestRow, ProcessColumns::Name, new QStandardItem(processName));
    processTableViewModel->setItem(newestRow, ProcessColumns::Notes, new QStandardItem(QString::number(newestRow + 1)));
    processTableViewModel->setItem(newestRow, ProcessColumns::Duration, new QStandardItem("00:00:00"));
    processTableViewModel->setItem(newestRow, ProcessColumns::LastSeen, new QStandardItem("Now"));
    processTableViewModel->setItem(newestRow, ProcessColumns::DateAdded,
        new QStandardItem(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));

    processDurations.insert(processName, 0);
}

void MainWindow::foundRunningProcess(QString processName, int row)
{
    processDurations[processName]++;
    processTableViewModel->setItem(row, ProcessColumns::Duration, new QStandardItem(Parser::parseDurationToString(processDurations[processName])));
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    qDebug() << index.column() << " " <<  index.row();
    return;
}
