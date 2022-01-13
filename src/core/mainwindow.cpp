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

/* If adding new column, modify:
 *
 * mystandarditemmodel.h - ProcessColumns
 *
 * mainwindow.cpp - MainWindow() - processTableViewModel->setHorizontalHeaderLabels ,
 *      loadProcessData() ,
 *      createProcessInTable() ,
 *      saveProcessData() ,
 *      newProcessAdded() ,
 *      exportSelectedRows() ,
 *      on_tableView_doubleClicked() and tableCellCustomContextMenuRequested() - optional ,
 *      on_actionImport_triggered()
 *
 * mainwindow.h - createProcessInTable()
 */

// SUPER IMPORTANT - use processTableViewModel only to add new rows, and processFilterProxyModel for everything else

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ProcessScanner *processScanner = new ProcessScanner();
    connect(this, &MainWindow::checkRunningProcesses, processScanner, &ProcessScanner::checkRunningProcesses);
    connect(processScanner, &ProcessScanner::foundRunningProcess, this, &MainWindow::foundRunningProcess);
    connect(processScanner, &ProcessScanner::foundStoppedProcesses, this, &MainWindow::foundStoppedProcesses);

    // Setup processes table
    ui->tableView->setItemDelegate(new MyItemDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList()
        << "#" << "Tracking" << "Icon" << "Name" << "Notes" << "Duration" << "Last seen" << "Date added");

    processFilterProxyModel = new MySortFilterProxyModel(this);
    processFilterProxyModel->setSourceModel(processTableViewModel);
    ui->tableView->setModel(processFilterProxyModel);

    // Setup cell context menu
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::tableCellCustomContextMenuRequested);

    // Setup horizontal header context menu
    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &MainWindow::tableHorizontalHeaderCustomContextMenuRequested);

    // Setup categories table
    categoriesTableModel->setHorizontalHeaderLabels(QStringList() << "Categories");
    ui->categoriesTable->setModel(categoriesTableModel);
    ui->categoriesTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);

    // Setup categories table context menu
    ui->categoriesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->categoriesTable, &QTableView::customContextMenuRequested, this, &MainWindow::dockCustomContextMenuRequested);

    // Setup line edit in toolbar for filtering process list
    QLineEdit *processFilterLineEdit = new QLineEdit(ui->toolBar);
    processFilterLineEdit->setPlaceholderText("🔍 Filter processes");
    connect(processFilterLineEdit, &QLineEdit::textChanged, this, &MainWindow::processFilterLineEdit_textChanged);
    processFilterLineEdit->setVisible(true);
    ui->toolBar->addWidget(processFilterLineEdit);

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

void MainWindow::loadProcessData()
{
    quicksettings("processList");
    foreach (QString processName, settings.childGroups())
    {
        settings.beginGroup(processName);

        processDurations.insert(processName, settings.value("duration", 0).toULongLong());

        createProcessInTable(
            settings.value("number").toString(),
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

void MainWindow::createProcessInTable(QString number, QString activeSymbol, QIcon icon, QString processName, QString notes, quint64 duration, QString lastSeen, QString dateAdded)
{
    processDurations.insert(processName, duration);

    int newestRow = processTableViewModel->rowCount();
    processTableViewModel->setItem(newestRow, ProcessColumns::Number, new MyStandardItem(number));
    processTableViewModel->setItem(newestRow, ProcessColumns::Tracking, new MyStandardItem(activeSymbol));
    processTableViewModel->setItem(newestRow, ProcessColumns::Icon, new MyStandardItem(icon, ""));
    processTableViewModel->setItem(newestRow, ProcessColumns::Name, new MyStandardItem(processName));
    processTableViewModel->setItem(newestRow, ProcessColumns::Notes, new MyStandardItem(notes));
    processTableViewModel->setItem(newestRow, ProcessColumns::Duration, new MyStandardItem(Parser::parseDurationToString(duration)));
    processTableViewModel->setItem(newestRow, ProcessColumns::LastSeen, new MyStandardItem(lastSeen));
    processTableViewModel->setItem(newestRow, ProcessColumns::DateAdded, new MyStandardItem(dateAdded));
}

QModelIndex MainWindow::getIndex(int row, int column)
{
    return processFilterProxyModel->index(row, column);
}

QVariant MainWindow::getIndexData(int row, int column)
{
    return processFilterProxyModel->data(processFilterProxyModel->index(row, column));
}

void MainWindow::saveProcessData()
{
    quicksettings("processList");
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
    {
        QString processName = getIndexData(row, ProcessColumns::Name).toString();
        settings.beginGroup(processName);

        settings.setValue("number", getIndexData(row, ProcessColumns::Number).toString());
        settings.setValue("tracking", getIndexData(row, ProcessColumns::Tracking).toString() == processIsActiveSymbol);
        settings.setValue("iconPath", processIcons[processName]);
        settings.setValue("notes", getIndexData(row, ProcessColumns::Notes).toString());
        settings.setValue("duration", processDurations[processName]);
        settings.setValue("lastSeen", getIndexData(row, ProcessColumns::LastSeen).toString());
        settings.setValue("dateAdded", getIndexData(row, ProcessColumns::DateAdded).toString());

        settings.endGroup();
    }
}

void MainWindow::loadWindowData()
{
    quicksettings("config");
    this->resize(settings.value("windowWidth", 1280).toUInt(), settings.value("windowHeight", 720).toUInt());
    processPollInterval = settings.value("processPollInterval", processPollInterval).toUInt();
    ui->tableView->horizontalHeader()->restoreState(settings.value("tableHorizontalHeader", "").toByteArray());
}

void MainWindow::saveWindowData()
{
    quicksettings("config");
    settings.setValue("windowWidth", this->width());
    settings.setValue("windowHeight", this->height());
    settings.setValue("processPollInterval", processPollInterval);
    settings.setValue("tableHorizontalHeader", ui->tableView->horizontalHeader()->saveState());
}

void MainWindow::pollProcesses()
{
    QMap<QString, int> processList;

    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        if (getIndexData(row, ProcessColumns::Tracking).toString() == processIsActiveSymbol)
            processList.insert(getIndexData(row, ProcessColumns::Name).toString(), row);

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
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
    {
        QString processName = getIndexData(row, ProcessColumns::Name).toString();
        if (runningProcesses.contains(processName))
        {
            processDurations[processName]++;
            processFilterProxyModel->setData(getIndex(row, ProcessColumns::Duration), Parser::parseDurationToString(processDurations[processName]));
            processFilterProxyModel->setData(getIndex(row, ProcessColumns::LastSeen), "Now");
        }
    }
}

void MainWindow::newProcessAdded(QString processName, QString iconPath)
{
    if (processAlreadyExists(processName))
    {
        systemTrayIcon->showMessage("Error", processName + " is already added", QSystemTrayIcon::Warning, 3000);
        return;
    }

    createProcessInTable(QString::number(processFilterProxyModel->rowCount() + 1),
        processIsActiveSymbol, getIcon(processName, iconPath), processName, "", 0, "Now", QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
}

bool MainWindow::processAlreadyExists(QString processName)
{
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        if (getIndexData(row, ProcessColumns::Name).toString() == processName)
            return true;

    return false;
}

void MainWindow::updateLastSeenIfRunningAndRemoveFromRunning(QString processName, int row)
{
    if (runningProcesses.contains(processName))
    {
        runningProcesses.removeAll(processName);
        processFilterProxyModel->setData(getIndex(row, ProcessColumns::LastSeen),
            QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    }
}

void MainWindow::updateLastSeenForRunningProcesses()
{
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        updateLastSeenIfRunningAndRemoveFromRunning(getIndexData(row, ProcessColumns::Name).toString(), row);
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
    QString processName = getIndexData(selectedRows.first().row(), ProcessColumns::Name).toString();

    int answer = getConfirmDialogAnswer("Confirm removal", QString("Remove %1? This action is irreversible!")
        .arg(selectedRows.size() == 1 ? processName : "multiple processes"));

    if (answer == QMessageBox::Yes)
    {
        quicksettings("processList");
        foreach(QModelIndex index, selectedRows)
        {
            QString currentProcessName = getIndexData(index.row(), ProcessColumns::Name).toString();
            settings.remove(currentProcessName);
            runningProcesses.removeAll(currentProcessName);
        }

        QModelIndexList indexes = selectedRows;
        while (!indexes.isEmpty())
        {
            processFilterProxyModel->removeRows(indexes.last().row(), 1);
            indexes.removeLast();
        }
    }

    normalizeProcessNumbers();
}

void MainWindow::normalizeProcessNumbers()
{
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        processFilterProxyModel->setData(getIndex(row, ProcessColumns::Number), row + 1);
}

void MainWindow::exportSelectedRows(QList<QModelIndex> selectedRows)
{
    QString exportLocation = QFileDialog::getSaveFileName(this, "Choose export location", "", "Text files (*.json)");
    if (exportLocation.isEmpty())
           return;

    if (!exportLocation.endsWith(".json", Qt::CaseInsensitive))
        exportLocation += ".json";

    QJsonObject processesJson;

    foreach (QModelIndex index, selectedRows)
    {
        QJsonObject processData;
        QString processName = getIndexData(index.row(), ProcessColumns::Name).toString();

        processData["tracking"] = getIndexData(index.row(), ProcessColumns::Tracking).toString() == processIsActiveSymbol;
        processData["iconPath"] = processIcons[processName];
        processData["notes"] = getIndexData(index.row(), ProcessColumns::Notes).toString();
        processData["duration"] = QJsonValue::fromVariant(processDurations[processName]);
        processData["lastSeen"] = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        processData["dateAdded"] = getIndexData(index.row(), ProcessColumns::DateAdded).toString();

        processesJson.insert(processName, processData);
    }

    QFile jsonFile(exportLocation);
    if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        jsonFile.write(QJsonDocument(processesJson).toJson(QJsonDocument::Indented));
        systemTrayIcon->showMessage("Processes exported", exportLocation, QSystemTrayIcon::Information, 3000);
    }
}

bool MainWindow::isJsonValid(QJsonObject jsonObject)
{
    if (jsonObject.size() >= 1)
    {
        foreach (QString processName, jsonObject.keys())
        {
            QJsonObject processData = jsonObject[processName].toObject();

            bool quint64ConversionSuccess;
            // If only QJsonObject had a toULongLong() method directly...
            Q_UNUSED(processData["duration"].toVariant().toString().toULongLong(&quint64ConversionSuccess));

            if (!processName.isEmpty()
                && processData["tracking"].isBool()
                && processData["iconPath"].isString()
                && processData["notes"].isString()
                && quint64ConversionSuccess
                && processData["lastSeen"].isString()
                && processData["dateAdded"].isString())
                    return true;
        }
    }

    return false;
}

/*---------------------------------------------------- User input ----------------------------------------------------*/

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString processName = getIndexData(index.row(), ProcessColumns::Name).toString();

    switch (index.column())
    {
        case ProcessColumns::Tracking:
        {
            updateLastSeenIfRunningAndRemoveFromRunning(processName, index.row());

            QString processState = getIndexData(index.row(), ProcessColumns::Tracking).toString();
            processFilterProxyModel->setData(getIndex(index.row(), ProcessColumns::Tracking),
                processState == processIsActiveSymbol ? processIsPausedSymbol : processIsActiveSymbol);

            break;
        }

        case ProcessColumns::Icon:
        {
            QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", Utility::imageFormats);
            QPixmap icon(fileName);

            if (!fileName.isEmpty() && !icon.isNull())
                processFilterProxyModel->setData(getIndex(index.row(), ProcessColumns::Icon), icon, Qt::DecorationRole);

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
            QString displayDuration = getIndexData(index.row(), ProcessColumns::Duration).toString();
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
    if (selectedRows.size() == 0)
        return;

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
                processFilterProxyModel->index(index.row(), actionName.second));});

        menu->addAction(action);
    }

    // Remove action
    QAction *action = new QAction("Remove", this);
    connect(action, &QAction::triggered, this, [=](){removeSelectedRows(selectedRows);});
    menu->addAction(action);

    // Export action
    action = new QAction("Export", this);
    connect(action, &QAction::triggered, this, [=](){exportSelectedRows(selectedRows);});
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

void MainWindow::dockCustomContextMenuRequested(const QPoint &pos)
{
    qDebug() << "Hello";
    QMenu *menu = new QMenu(this);
    menu->addAction(new QAction("Hello there", this));

    menu->popup(ui->categoriesTable->viewport()->mapToGlobal(pos));
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

void MainWindow::on_actionImport_triggered()
{
    QStringList importLocations = QFileDialog::getOpenFileNames(this, "Choose import files", "", "Text files (*.json)");
    foreach (QString importLocation, importLocations)
    {
        QFile jsonFile(importLocation);
        QJsonObject jsonObject;
        if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
            jsonObject = QJsonDocument::fromJson(jsonFile.readAll()).object();

        if (!isJsonValid(jsonObject))
        {
            systemTrayIcon->showMessage("Error invalid Json", importLocation, QSystemTrayIcon::Warning, 3000);
            continue;
        }

        foreach (QString processName, jsonObject.keys())
        {
            if (processAlreadyExists(processName))
            {
                systemTrayIcon->showMessage("Error", processName + " is already added", QSystemTrayIcon::Warning, 3000);
                continue;
            }

            QJsonObject processData = jsonObject[processName].toObject();

            createProcessInTable(
                QString::number(processFilterProxyModel->rowCount() + 1),
                processData["tracking"].toBool() ? processIsActiveSymbol : processIsPausedSymbol,
                getIcon(processName, processData["iconPath"].toString()),
                processName,
                processData["notes"].toString(),
                processData["duration"].toVariant().toULongLong(),
                processData["lastSeen"].toString(),
                processData["dateAdded"].toString());
        }
    }
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

void MainWindow::processFilterLineEdit_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
    {
        processFilterProxyModel->setFilterFixedString("");
    }
    else
    {
        processFilterProxyModel->setFilterRegularExpression(QRegularExpression(arg1, QRegularExpression::CaseInsensitiveOption));
    }

    processFilterProxyModel->setFilterKeyColumn(-1); // -1 means all columns
}

void MainWindow::systemTrayIconActionOpen()
{
    this->show();
}

void MainWindow::systemTrayIconActionResumeAll()
{
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        processFilterProxyModel->setData(getIndex(row, ProcessColumns::Tracking), processIsActiveSymbol);
}

void MainWindow::systemTrayIconActionPauseAll()
{
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        processFilterProxyModel->setData(getIndex(row, ProcessColumns::Tracking), processIsPausedSymbol);
}

void MainWindow::systemTrayIconActionExit()
{
    qApp->quit();
}
