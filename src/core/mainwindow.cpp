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
    connect(processScanner, &ProcessScanner::foundStoppedProcesses, this, &MainWindow::foundStoppedProcesses);

    ui->tableView->setItemDelegate(new MyItemDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList() << "Tracking" << "Icon" << "Name" << "Notes" << "Duration" << "Last seen" << "Date added");
    ui->tableView->setModel(processTableViewModel);
    //TODO remove after manually saving column widths
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QTimer *processPollTimer = new QTimer(this);
    processPollTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(processPollTimer, &QTimer::timeout, this, &MainWindow::pollProcesses);
    processPollTimer->start(processPollInterval);

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

}

void MainWindow::pollProcesses()
{
    QStringList processList;

    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        processList.append(processTableViewModel->item(row, ProcessColumns::Name)->text());

    emit checkRunningProcesses(processList);
}

void MainWindow::foundRunningProcess(QString processName)
{
    runningProcesses.append(processName);
}

void MainWindow::foundStoppedProcesses(QStringList stoppedProcesses)
{
    foreach (QString ProcessName, stoppedProcesses)
        runningProcesses.removeAll(ProcessName);
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

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    switch (index.column())
    {
        case ProcessColumns::Tracking:
        {
            QString processState = processTableViewModel->item(index.row(), ProcessColumns::Tracking)->text();
            processTableViewModel->setItem(index.row(), ProcessColumns::Tracking,
                new QStandardItem(processState == processIsActiveSymbol ? processIsPausedSymbol : processIsActiveSymbol));

            break;
        }

        case ProcessColumns::Icon:
        {
            QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", Utility::imageFormats);
            QPixmap icon(fileName);

            if (!fileName.isEmpty() && !icon.isNull())
                processTableViewModel->setItem(index.row(), ProcessColumns::Icon, new QStandardItem(icon, ""));

            break;
        }

        case ProcessColumns::Name:
        {
            QMessageBox confirmDialog(this);
            confirmDialog.setWindowTitle("Confirm removal");
            confirmDialog.setText(QString("Are you sure you wish to remove %1?").arg(processTableViewModel->item(index.row(), ProcessColumns::Name)->text()));
            confirmDialog.setIcon(QMessageBox::Question);
            confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            confirmDialog.setDefaultButton(QMessageBox::No);
            int answer = confirmDialog.exec();

            if (answer == QMessageBox::Yes)
                processTableViewModel->removeRow(index.row());

            break;
        }

        case ProcessColumns::Duration:
        {
            bool ok;
            QString displayDuration = processTableViewModel->item(index.row(), ProcessColumns::Duration)->text();
            QString durationInput = QInputDialog::getText(this, "Custom duration", "Set custom duration", QLineEdit::Normal, displayDuration, &ok);

            if (ok)
                processDurations[processTableViewModel->item(index.row(), ProcessColumns::Name)->text()] = Parser::parseStringToDuration(durationInput);

            break;
        }
    }
}

void MainWindow::on_actionAdd_triggered()
{
    ProcessDialog *processDialog = new ProcessDialog();
    connect(processDialog, &ProcessDialog::newProcessAdded, this, &MainWindow::newProcessAdded);
    processDialog->exec();
}

void MainWindow::on_actionPoll_triggered()
{
    pollProcesses();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}
