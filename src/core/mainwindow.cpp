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

    // Setup table
    ui->tableView->setItemDelegate(new MyItemDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList() << "Tracking" << "Icon" << "Name" << "Notes" << "Duration" << "Last seen" << "Date added");
    ui->tableView->setModel(processTableViewModel);

    // Setup cell context menu
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::tableCellCustomContextMenuRequested);

    // Setup horizontal header
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &MainWindow::tableHorizontalHeaderCustomContextMenuRequested);

    // Setup vertical header
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &MainWindow::tableVerticalHeaderCustomContextMenuRequested);

    loadProcessData();
    loadWindowData();
    pollProcesses();

    // Background timers
    processPollTimer = new QTimer(this);
    processPollTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(processPollTimer, &QTimer::timeout, this, &MainWindow::pollProcesses);
    processPollTimer->start(processPollInterval);

    runningProcessDurationsUpdateTimer = new QTimer(this);
    runningProcessDurationsUpdateTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(runningProcessDurationsUpdateTimer, &QTimer::timeout, this, &MainWindow::updateRunningProcessDurations);
    runningProcessDurationsUpdateTimer->start(processSecondIncrementInterval);

    processDataAutoSaveTimer = new QTimer(this);
    processDataAutoSaveTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(processDataAutoSaveTimer, &QTimer::timeout, this, &MainWindow::saveProcessData);
    processDataAutoSaveTimer->start(processAutoSaveInterval);

    // Create system tray icon
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/app-icon.svg"));

    QMenu *systemTrayIconMenu = new QMenu();
    systemTrayIconMenu->addAction("Open", this, &MainWindow::systemTrayIconActionOpen);
    systemTrayIconMenu->addAction("Resume all", this, &MainWindow::systemTrayIconActionResumeAll);
    systemTrayIconMenu->addAction("Pause all", this, &MainWindow::systemTrayIconActionPauseAll);
    systemTrayIconMenu->addAction("Show all", this, &MainWindow::systemTrayIconActionShowAll);
    systemTrayIconMenu->addAction("Hide all", this, &MainWindow::systemTrayIconActionHideAll);
    systemTrayIconMenu->addAction("Exit", this, &MainWindow::systemTrayIconActionExit);

    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::systemTrayIconActionOpen);

    systemTrayIcon->setContextMenu(systemTrayIconMenu);
    systemTrayIcon->show();
}

MainWindow::~MainWindow()
{
    processPollTimer->stop();
    runningProcessDurationsUpdateTimer->stop();
    processDataAutoSaveTimer->stop();
    updateLastSeenForRunningProcesses();
    saveProcessData();
    saveWindowData();
    delete ui;
}

void MainWindow::on_actionDebug_triggered()
{

}

void MainWindow::loadProcessData()
{
    quicksettings("processList");
    foreach (QString processName, settings.childGroups())
    {
        settings.beginGroup(processName);

        processDurations.insert(processName, settings.value("duration", 0).toULongLong());

        createProcessInTable(
            settings.value("tracking", true).toBool() ? processIsActiveSymbol : processIsPausedSymbol,
            getIcon(processName, settings.value("iconPath").toString()),
            processName,
            settings.value("notes").toString(),
            processDurations[processName],
            settings.value("lastSeen").toString(),
            settings.value("dateAdded").toString());

        settings.endGroup();
    }
}

QIcon MainWindow::getIcon(QString processName, QString iconPath)
{
    processIcons[processName] = iconPath.isEmpty() ? ":/app-icon.svg" : iconPath;
    return QIcon(processIcons[processName]);
}

void MainWindow::createProcessInTable(QString activeSymbol, QIcon icon, QString processName, QString notes, quint64 duration, QString lastSeen, QString dateAdded)
{
    processDurations.insert(processName, duration);

    int newestRow = processTableViewModel->rowCount();
    processTableViewModel->setItem(newestRow, ProcessColumns::Tracking, new MyStandardItem(activeSymbol));
    processTableViewModel->setItem(newestRow, ProcessColumns::Icon, new MyStandardItem(icon, ""));
    processTableViewModel->setItem(newestRow, ProcessColumns::Name, new MyStandardItem(processName));
    processTableViewModel->setItem(newestRow, ProcessColumns::Notes, new MyStandardItem(notes));
    processTableViewModel->setItem(newestRow, ProcessColumns::Duration, new MyStandardItem(Parser::parseDurationToString(duration)));
    processTableViewModel->setItem(newestRow, ProcessColumns::LastSeen, new MyStandardItem(lastSeen));
    processTableViewModel->setItem(newestRow, ProcessColumns::DateAdded, new MyStandardItem(dateAdded));
}

void MainWindow::saveProcessData()
{
    quicksettings("processList");
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        QString processName = processTableViewModel->item(row, ProcessColumns::Name)->text();
        settings.beginGroup(processName);

        settings.setValue("tracking", processTableViewModel->item(row, ProcessColumns::Tracking)->text() == processIsActiveSymbol);
        settings.setValue("iconPath", processIcons[processName]);
        settings.setValue("notes", processTableViewModel->item(row, ProcessColumns::Notes)->text());
        settings.setValue("duration", processDurations[processName]);
        settings.setValue("lastSeen", processTableViewModel->item(row, ProcessColumns::LastSeen)->text());
        settings.setValue("dateAdded", processTableViewModel->item(row, ProcessColumns::DateAdded)->text());

        settings.endGroup();
    }
}

void MainWindow::loadWindowData()
{
    quicksettings("config");
    this->resize(settings.value("windowWidth", 1280).toUInt(), settings.value("windowHeight", 720).toUInt());
    processPollInterval = settings.value("processPollInterval", processPollInterval).toUInt();
    ui->tableView->horizontalHeader()->restoreState(settings.value("tableHorizontalHeader", "").toByteArray());
    ui->tableView->verticalHeader()->restoreState(settings.value("tableVerticalHeader", "").toByteArray());
}

void MainWindow::saveWindowData()
{
    quicksettings("config");
    settings.setValue("windowWidth", this->width());
    settings.setValue("windowHeight", this->height());
    settings.setValue("processPollInterval", processPollInterval);
    settings.setValue("tableHorizontalHeader", ui->tableView->horizontalHeader()->saveState());
    settings.setValue("tableVerticalHeader", ui->tableView->verticalHeader()->saveState());
}

void MainWindow::pollProcesses()
{
    QMap<QString, int> processList;

    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        if (processTableViewModel->item(row, ProcessColumns::Tracking)->text() == processIsActiveSymbol)
            processList.insert(processTableViewModel->item(row, ProcessColumns::Name)->text(), row);

    emit checkRunningProcesses(processList);
}

void MainWindow::foundRunningProcess(QString processName)
{
    runningProcesses.append(processName);
}

void MainWindow::foundStoppedProcesses(QMap<QString, int> stoppedProcesses)
{
    foreach (QString processName, stoppedProcesses.keys())
        updateLastSeenIfRunningAndRemoveFromRunning(processName, stoppedProcesses[processName]);
}

void MainWindow::updateRunningProcessDurations()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        QString processName = processTableViewModel->item(row, ProcessColumns::Name)->text();
        if (runningProcesses.contains(processName))
        {
            processDurations[processName]++;
            processTableViewModel->setItem(row, ProcessColumns::Duration, new MyStandardItem(Parser::parseDurationToString(processDurations[processName])));
            processTableViewModel->setItem(row, ProcessColumns::LastSeen, new MyStandardItem("Now"));
        }
    }
}

void MainWindow::newProcessAdded(QString processName, QString iconPath)
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        if (processTableViewModel->item(row, ProcessColumns::Name)->text() == processName)
        {
            systemTrayIcon->showMessage("Error", processName + " is already added", QSystemTrayIcon::Warning, 3000);
            return;
        }
    }

    createProcessInTable(processIsActiveSymbol, getIcon(processName, iconPath), processName,
        QString::number(processTableViewModel->rowCount() + 1), 0, "Now", QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
}

void MainWindow::updateLastSeenIfRunningAndRemoveFromRunning(QString processName, int row)
{
    if (runningProcesses.contains(processName))
    {
        runningProcesses.removeAll(processName);
        processTableViewModel->setItem(row, ProcessColumns::LastSeen,
            new MyStandardItem(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));
    }
}

void MainWindow::updateLastSeenForRunningProcesses()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        updateLastSeenIfRunningAndRemoveFromRunning(processTableViewModel->item(row, ProcessColumns::Name)->text(), row);
}

void MainWindow::userOptionsChosen(uint processPollInterval)
{
    uint pollInterval = processPollInterval * 1000;
    this->processPollInterval = pollInterval;
    processPollTimer->setInterval(pollInterval);
}

int MainWindow::getConfirmDialogAnswer(QString title, QString text)
{
    QMessageBox confirmDialog(this);
    confirmDialog.setWindowTitle(title);
    confirmDialog.setText(text);
    confirmDialog.setIcon(QMessageBox::Question);
    confirmDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmDialog.setDefaultButton(QMessageBox::No);
    return confirmDialog.exec();
}

void MainWindow::removeSelectedRows(QList<QModelIndex> selectedRows)
{
    QString processName = processTableViewModel->item(selectedRows.first().row(), ProcessColumns::Name)->text();

    int answer = getConfirmDialogAnswer("Confirm removal", QString("Remove %1? This action is irreversible!")
        .arg(selectedRows.size() == 1 ? processName : "multiple processes"));

    if (answer == QMessageBox::Yes)
    {
        QModelIndexList indexes = selectedRows;
        quicksettings("processList");
        while (!indexes.isEmpty())
        {
            settings.remove(processName);
            processTableViewModel->removeRows(indexes.last().row(), 1);
            indexes.removeLast();
        }
    }
}

/*---------------------------------------------------- User input ----------------------------------------------------*/

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString processName = processTableViewModel->item(index.row(), ProcessColumns::Name)->text();

    switch (index.column())
    {
        case ProcessColumns::Tracking:
        {
            updateLastSeenIfRunningAndRemoveFromRunning(processName, index.row());

            QString processState = processTableViewModel->item(index.row(), ProcessColumns::Tracking)->text();
            processTableViewModel->setItem(index.row(), ProcessColumns::Tracking,
                new MyStandardItem(processState == processIsActiveSymbol ? processIsPausedSymbol : processIsActiveSymbol));

            break;
        }

        case ProcessColumns::Icon:
        {
            QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", Utility::imageFormats);
            QPixmap icon(fileName);

            if (!fileName.isEmpty() && !icon.isNull())
                processTableViewModel->setItem(index.row(), ProcessColumns::Icon, new MyStandardItem(icon, ""));

            break;
        }

        case ProcessColumns::Name:
        {
            removeSelectedRows(QList<QModelIndex>() << index);

            break;
        }

        case ProcessColumns::Duration:
        {
            bool ok;
            QString displayDuration = processTableViewModel->item(index.row(), ProcessColumns::Duration)->text();
            QString durationInput = QInputDialog::getText(this, "Custom duration", "Set custom duration", QLineEdit::Normal, displayDuration, &ok);

            if (ok)
                processDurations[processName] = Parser::parseStringToDuration(durationInput);

            break;
        }
    }
}

void MainWindow::tableCellCustomContextMenuRequested(const QPoint &pos)
{
    QList<QModelIndex> selectedRows = ui->tableView->selectionModel()->selectedRows();

    QList<QPair<QString, ProcessColumns::ProcessColumns>> actionNames = {{"Resume / Pause", ProcessColumns::Tracking}};
    if (selectedRows.size() == 1)
    {
        actionNames.append({"Change icon", ProcessColumns::Icon});
        actionNames.append({"Change duration", ProcessColumns::Duration});
    }

    QMenu *menu = new QMenu(this);

    //If multiple rows are selected, one action is displayed which is connected to all the selected rows
    foreach (auto actionName, actionNames)
    {
        QAction *action = new QAction(actionName.first, this);

        foreach (QModelIndex index, selectedRows)
            connect(action, &QAction::triggered, this, [=](){on_tableView_doubleClicked(
                processTableViewModel->indexFromItem(processTableViewModel->item(index.row(), actionName.second)));});

        menu->addAction(action);
    }

    QAction *action = new QAction("Remove", this);
    connect(action, &QAction::triggered, this, [=](){removeSelectedRows(selectedRows);});

    menu->addAction(action);

    action = new QAction("Hide", this);
    foreach (QModelIndex index, selectedRows)
        connect(action, &QAction::triggered, this, [=](){ui->tableView->hideRow(index.row());});

    menu->addAction(action);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::tableHorizontalHeaderCustomContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    for (int column = 0; column < processTableViewModel->columnCount(); column++)
    {
        QAction *action = new QAction(processTableViewModel->horizontalHeaderItem(column)->text(), this);
        action->setCheckable(true);
        action->setChecked(!ui->tableView->isColumnHidden(column));

        connect(action, &QAction::triggered, this, [=](bool checked)
        {
            if (!checked)
                ui->tableView->hideColumn(column);
            else
                ui->tableView->showColumn(column);
        });

        menu->addAction(action);
    }

    menu->popup(ui->tableView->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void MainWindow::tableVerticalHeaderCustomContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        QAction *action = new QAction(processTableViewModel->item(row, ProcessColumns::Name)->text(), this);
        action->setCheckable(true);
        action->setChecked(!ui->tableView->isRowHidden(row));

        connect(action, &QAction::triggered, this, [=](bool checked)
        {
            if (!checked)
                ui->tableView->hideRow(row);
            else
                ui->tableView->showRow(row);
        });

        menu->addAction(action);
    }

    menu->popup(ui->tableView->verticalHeader()->viewport()->mapToGlobal(pos));
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

void MainWindow::on_actionInvert_hidden_triggered()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
    {
        if (ui->tableView->isRowHidden(row))
            ui->tableView->showRow(row);
        else
            ui->tableView->hideRow(row);
    }
}

void MainWindow::on_actionStretch_triggered()
{
    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
}

void MainWindow::on_actionOptions_triggered()
{
    Options *options = new Options(nullptr, processPollInterval / 1000);
    connect(options, &Options::userOptionsChosen, this, &MainWindow::userOptionsChosen);
    options->show();
}

void MainWindow::on_actionHelp_triggered()
{
    About *about = new About();
    about->show();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::systemTrayIconActionOpen()
{
    this->show();
}

void MainWindow::systemTrayIconActionResumeAll()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        processTableViewModel->setItem(row, ProcessColumns::Tracking, new MyStandardItem(processIsActiveSymbol));
}

void MainWindow::systemTrayIconActionPauseAll()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        processTableViewModel->setItem(row, ProcessColumns::Tracking, new MyStandardItem(processIsPausedSymbol));
}

void MainWindow::systemTrayIconActionShowAll()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        ui->tableView->showRow(row);
}

void MainWindow::systemTrayIconActionHideAll()
{
    for (int row = 0; row < processTableViewModel->rowCount(); row++)
        ui->tableView->hideRow(row);
}

void MainWindow::systemTrayIconActionExit()
{
    qApp->quit();
}
