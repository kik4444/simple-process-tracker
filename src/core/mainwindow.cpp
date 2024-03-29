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
 *
 *
 * SUPER IMPORTANT - use processTableViewModel only to add new rows, and processFilterProxyModel for everything else
 */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    #ifdef Q_OS_WINDOWS

    {
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        if (settings.value("AppsUseLightTheme") == 0)
        {
            qApp->setStyle(QStyleFactory::create("Fusion"));
            QPalette darkPalette;
            QColor darkColor = QColor(45, 45, 45);
            QColor disabledColor = QColor(127, 127, 127);
            darkPalette.setColor(QPalette::Window, darkColor);
            darkPalette.setColor(QPalette::WindowText, Qt::white);
            darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
            darkPalette.setColor(QPalette::AlternateBase, darkColor);
            darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
            darkPalette.setColor(QPalette::ToolTipText, Qt::white);
            darkPalette.setColor(QPalette::Text, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
            darkPalette.setColor(QPalette::Button, darkColor);
            darkPalette.setColor(QPalette::ButtonText, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::PlaceholderText, QColor(160, 160, 160));
            darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

            darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

            qApp->setPalette(darkPalette);

            qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
        }
    }

    #endif

    ProcessScanner *processScanner = new ProcessScanner();
    connect(this, &MainWindow::checkRunningProcesses, processScanner, &ProcessScanner::checkRunningProcesses);
    connect(processScanner, &ProcessScanner::foundRunningProcess, this, &MainWindow::foundRunningProcess);
    connect(processScanner, &ProcessScanner::foundStoppedProcesses, this, &MainWindow::foundStoppedProcesses);

    // Setup processes table
    ui->tableView->setItemDelegate(new MyItemDelegate());
    processTableViewModel->setHorizontalHeaderLabels(QStringList()
        << "Categories" << "#" << "Tracking" << "Icon" << "Name" << "Duration" << "Notes" << "Last seen" << "Date added");

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
    categoriesTableModel->setHorizontalHeaderLabels(QStringList() << "#" << "Categories");
    ui->categoriesTable->setModel(categoriesTableModel);
    ui->categoriesTable->setItemDelegate(new MyItemDelegate());
    ui->categoriesTable->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);

    // Setup categories table context menu
    ui->categoriesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->categoriesTable, &QTableView::customContextMenuRequested, this, &MainWindow::categoriesTableCustomContextMenuRequested);

    // Setup line edit in toolbar for filtering process list
    QLineEdit *processFilterLineEdit = new QLineEdit(ui->toolBar);
    processFilterLineEdit->setPlaceholderText("🔍 Filter processes");
    connect(processFilterLineEdit, &QLineEdit::textChanged, this, &MainWindow::processFilterLineEdit_textChanged);
    processFilterLineEdit->setVisible(true);
    ui->toolBar->addWidget(processFilterLineEdit);

    // Setup changing toolbar style on layout change
    for (QToolBar *toolbar : {ui->toolBar, ui->toolBar_2})
        connect(toolbar, &QToolBar::orientationChanged, this, [=](Qt::Orientation orientation)
        {
            toolbar->setToolButtonStyle(orientation == Qt::Vertical ? Qt::ToolButtonTextUnderIcon : Qt::ToolButtonTextBesideIcon);
        });

    // Hide empty space in top of categories dock widget
    ui->categoriesDock->setTitleBarWidget(new QWidget());

    loadProcessData();
    loadCategoryData();
    loadWindowData();
    pollProcesses();

    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setDragDropMode(QTableView::InternalMove);

    #ifdef DEBUG_MODE
    ui->tableView->showColumn(ProcessColumns::HiddenCategories);
    ui->categoriesTable->showColumn(CategoryColumns::HiddenNumber);
    ui->actionDebug->setVisible(true);
    #else
    ui->tableView->hideColumn(ProcessColumns::HiddenCategories);
    ui->categoriesTable->hideColumn(CategoryColumns::HiddenNumber);
    #endif

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
    systemTrayIcon->setIcon(QIcon(defaultProcessIcon));

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
    saveCategoryData();
    saveWindowData();
    delete ui;
}

#ifdef DEBUG_MODE
void MainWindow::on_actionDebug_triggered()
{
    qDebug() << QFontDatabase::families();
}
#endif

void MainWindow::loadProcessData()
{
    quicksettings("processList");
    foreach (QString processName, settings.childGroups())
    {
        settings.beginGroup(processName);

        processDurations.insert(processName, settings.value("duration", 0).toULongLong());

        createProcessInTable(
            settings.value("categories").toString(),
            settings.value("number").toString(),
            settings.value("tracking", true).toBool() ? processIsActiveSymbol : processIsPausedSymbol,
            getIcon(processName, settings.value("iconPath").toString()),
            processName,
            processDurations[processName],
            settings.value("notes").toString(),
            settings.value("lastSeen").toString(),
            settings.value("dateAdded").toString());

        settings.endGroup();
    }
}

void MainWindow::loadCategoryData()
{
    quicksettings("categories");
    foreach (QString categoryName, settings.childGroups())
        createCategoryInTable(settings.value(categoryName + "/number", categoriesTableModel->rowCount() + 1).toUInt(), categoryName);

    ui->categoriesTable->sortByColumn(CategoryColumns::HiddenNumber, Qt::AscendingOrder);
}

void MainWindow::saveCategoryData()
{
    quicksettings("categories");
    settings.clear();
    for (int row = 0; row < categoriesTableModel->rowCount(); row++)
    {
        uint categoryNumber = categoriesTableModel->data(categoriesTableModel->index(row, CategoryColumns::HiddenNumber)).toUInt();
        QString categoryName = categoriesTableModel->data(categoriesTableModel->index(row, CategoryColumns::Name)).toString();
        settings.setValue(categoryName + "/number", categoryNumber);
    }
}

QIcon MainWindow::getIcon(QString processName, QString iconPath)
{
    processIcons[processName] = iconPath.isEmpty() ? defaultProcessIcon : iconPath;
    return QIcon(processIcons[processName]);
}

void MainWindow::createProcessInTable(QString categories, QString number, QString activeSymbol, QIcon icon, QString processName, quint64 duration, QString notes, QString lastSeen, QString dateAdded)
{
    processDurations.insert(processName, duration);

    int newestRow = processFilterProxyModel->sourceModel()->rowCount();
    processTableViewModel->setItem(newestRow, ProcessColumns::HiddenCategories, new MyStandardItem(categories));
    processTableViewModel->setItem(newestRow, ProcessColumns::Number, new MyStandardItem(number));
    processTableViewModel->setItem(newestRow, ProcessColumns::Tracking, new MyStandardItem(activeSymbol));
    processTableViewModel->setItem(newestRow, ProcessColumns::Icon, new MyStandardItem(icon, ""));
    processTableViewModel->setItem(newestRow, ProcessColumns::Name, new MyStandardItem(processName));
    processTableViewModel->setItem(newestRow, ProcessColumns::Duration, new MyStandardItem(Parser::parseDurationToString(duration)));
    processTableViewModel->setItem(newestRow, ProcessColumns::Notes, new MyStandardItem(notes));
    processTableViewModel->setItem(newestRow, ProcessColumns::LastSeen, new MyStandardItem(lastSeen));
    processTableViewModel->setItem(newestRow, ProcessColumns::DateAdded, new MyStandardItem(dateAdded));
}

void MainWindow::createCategoryInTable(uint categoryNumber, QString categoryName)
{
    if (!categoryName.isEmpty())
    {
        int newestRow = categoriesTableModel->rowCount();
        categoriesTableModel->setItem(newestRow, CategoryColumns::HiddenNumber, new MyStandardItem(QString::number(categoryNumber)));
        categoriesTableModel->setItem(newestRow, CategoryColumns::Name, new MyStandardItem(categoryName));
    }
}

QModelIndex MainWindow::getIndex(int row, int column)
{
    return processFilterProxyModel->index(row, column);
}

QVariant MainWindow::getIndexData(int row, int column)
{
    return processFilterProxyModel->data(processFilterProxyModel->index(row, column));
}

QModelIndex MainWindow::getRealIndex(int row, int column)
{
    return processFilterProxyModel->sourceModel()->index(row, column);
}

QVariant MainWindow::getRealIndexData(int row, int column)
{
    return processFilterProxyModel->sourceModel()->data(processFilterProxyModel->sourceModel()->index(row, column));
}

QStringList MainWindow::getDelimitedCategories()
{
    QStringList categories;
    for (int row = 0; row < categoriesTableModel->rowCount(); row++)
        categories.append(categoriesTableModel->item(row, CategoryColumns::Name)->text());

    return categories;
}

void MainWindow::saveProcessData()
{
    quicksettings("processList");
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
    {
        QString processName = getRealIndexData(row, ProcessColumns::Name).toString();
        settings.beginGroup(processName);

        settings.setValue("categories", getRealIndexData(row, ProcessColumns::HiddenCategories).toString());
        settings.setValue("number", getRealIndexData(row, ProcessColumns::Number).toString());
        settings.setValue("tracking", getRealIndexData(row, ProcessColumns::Tracking).toString() == processIsActiveSymbol);
        settings.setValue("iconPath", processIcons[processName]);
        settings.setValue("duration", processDurations[processName]);
        settings.setValue("notes", getRealIndexData(row, ProcessColumns::Notes).toString());
        settings.setValue("lastSeen", getRealIndexData(row, ProcessColumns::LastSeen).toString());
        settings.setValue("dateAdded", getRealIndexData(row, ProcessColumns::DateAdded).toString());

        settings.endGroup();
    }
}

void MainWindow::loadWindowData()
{
    quicksettings("config");
    this->resize(settings.value("windowWidth", 1280).toUInt(), settings.value("windowHeight", 720).toUInt());
    this->resizeDocks({ui->categoriesDock}, {settings.value("categoriesDockWidth", 300).toInt()}, Qt::Horizontal);
    processPollInterval = settings.value("processPollInterval", processPollInterval).toUInt();
    ui->tableView->horizontalHeader()->restoreState(settings.value("tableHorizontalHeader", "").toByteArray());
    restoreTableFilterState(settings.value("lastCategoryRow", -1).toInt());
}

void MainWindow::saveWindowData()
{
    quicksettings("config");
    settings.setValue("windowWidth", this->width());
    settings.setValue("windowHeight", this->height());
    settings.setValue("categoriesDockWidth", ui->categoriesDock->width());
    settings.setValue("processPollInterval", processPollInterval);
    settings.setValue("tableHorizontalHeader", ui->tableView->horizontalHeader()->saveState());
    settings.setValue("lastCategoryRow", currentlySelectedCategoriesRow);
}

void MainWindow::pollProcesses()
{
    QMap<QString, int> realProcessList;

    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        if (getRealIndexData(row, ProcessColumns::Tracking).toString() == processIsActiveSymbol)
            realProcessList.insert(getRealIndexData(row, ProcessColumns::Name).toString(), row);

    emit checkRunningProcesses(realProcessList);
}

void MainWindow::foundRunningProcess(QString processName)
{
    runningProcesses.append(processName);
}

void MainWindow::foundStoppedProcesses(QMap<QString, int> realStoppedProcesses)
{
    foreach (QString processName, realStoppedProcesses.keys())
        updateLastSeenIfRunningAndRemoveFromRunning(processName, realStoppedProcesses[processName]);
}

void MainWindow::updateRunningProcessDurations()
{
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
    {
        QString processName = getRealIndexData(row, ProcessColumns::Name).toString();
        if (runningProcesses.contains(processName))
        {
            processDurations[processName]++;
            processFilterProxyModel->sourceModel()->setData(getRealIndex(row, ProcessColumns::Duration), Parser::parseDurationToString(processDurations[processName]));
            processFilterProxyModel->sourceModel()->setData(getRealIndex(row, ProcessColumns::LastSeen), "Now");
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

    createProcessInTable("", QString::number(processFilterProxyModel->sourceModel()->rowCount() + 1),
        processIsActiveSymbol, getIcon(processName, iconPath), processName, 0, "", "Now", QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
}

bool MainWindow::processAlreadyExists(QString processName)
{
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        if (getRealIndexData(row, ProcessColumns::Name).toString() == processName)
            return true;

    return false;
}

void MainWindow::updateLastSeenIfRunningAndRemoveFromRunning(QString processName, int row)
{
    if (runningProcesses.contains(processName))
    {
        runningProcesses.removeAll(processName);
        processFilterProxyModel->sourceModel()->setData(getRealIndex(row, ProcessColumns::LastSeen),
            QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    }
}

void MainWindow::updateLastSeenForRunningProcesses()
{
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        updateLastSeenIfRunningAndRemoveFromRunning(getRealIndexData(row, ProcessColumns::Name).toString(), row);
}

void MainWindow::setProcessesPaused(QList<QModelIndex> realProcesses, bool paused)
{
    foreach (QModelIndex index, realProcesses)
    {
        updateLastSeenIfRunningAndRemoveFromRunning(getRealIndexData(index.row(), ProcessColumns::Name).toString(), index.row());
        processFilterProxyModel->sourceModel()->setData(getRealIndex(index.row(), ProcessColumns::Tracking), paused ? processIsPausedSymbol : processIsActiveSymbol);
    }
}

void MainWindow::userOptionsChosen(uint processPollInterval)
{
    this->processPollInterval = processPollInterval;
    processPollTimer->setInterval(processPollInterval);
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

void MainWindow::removeSelectedRows(QList<QModelIndex> proxySelectedRows)
{
    QString processName = getIndexData(proxySelectedRows.first().row(), ProcessColumns::Name).toString();

    int answer = getConfirmDialogAnswer("Confirm removal", QString("Remove %1? This action is irreversible!")
        .arg(proxySelectedRows.size() == 1 ? processName : "multiple processes"));

    if (answer == QMessageBox::Yes)
    {
        quicksettings("processList");
        foreach(QModelIndex index, proxySelectedRows)
        {
            QString currentProcessName = getIndexData(index.row(), ProcessColumns::Name).toString();
            settings.remove(currentProcessName);
            runningProcesses.removeAll(currentProcessName);
        }

        QList<QModelIndex> indexes = proxySelectedRows;
        std::sort(indexes.begin(), indexes.end());
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
    QList<QModelIndex> realIndexNumbers;
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        realIndexNumbers.append(getRealIndex(row, ProcessColumns::Number));

    std::sort(realIndexNumbers.begin(), realIndexNumbers.end(), MainWindow::compareQModelIndexData);

    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        processFilterProxyModel->sourceModel()->setData(realIndexNumbers.at(row), row + 1);
}

bool MainWindow::compareQModelIndexData(const QModelIndex &left, const QModelIndex &right)
{
    return left.data().toUInt() < right.data().toUInt();
}

void MainWindow::exportProcesses(QList<QModelIndex> realProcesses)
{
    QString exportLocation = QFileDialog::getSaveFileName(this, "Choose export location", "", "Text files (*.json)");
    if (exportLocation.isEmpty())
           return;

    if (!exportLocation.endsWith(".json", Qt::CaseInsensitive))
        exportLocation += ".json";

    QJsonObject processesJson;

    foreach (QModelIndex index, realProcesses)
    {
        QJsonObject processData;
        QString processName = getRealIndexData(index.row(), ProcessColumns::Name).toString();
        QString lastSeenTime = getRealIndexData(index.row(), ProcessColumns::LastSeen).toString();

        processData["categories"] = getRealIndexData(index.row(), ProcessColumns::HiddenCategories).toString();
        processData["tracking"] = getRealIndexData(index.row(), ProcessColumns::Tracking).toString() == processIsActiveSymbol;
        processData["iconPath"] = processIcons[processName];
        processData["duration"] = QJsonValue::fromVariant(processDurations[processName]);
        processData["notes"] = getRealIndexData(index.row(), ProcessColumns::Notes).toString();
        processData["lastSeen"] = lastSeenTime == "Now" ? QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") : lastSeenTime;
        processData["dateAdded"] = getRealIndexData(index.row(), ProcessColumns::DateAdded).toString();

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
                && quint64ConversionSuccess
                && processData["notes"].isString()
                && processData["lastSeen"].isString()
                && processData["dateAdded"].isString())
                    return true;
        }
    }

    return false;
}

bool MainWindow::categoryAlreadyExists(QString category)
{
    for (int row = 0; row < categoriesTableModel->rowCount(); row++)
        if (categoriesTableModel->item(row, CategoryColumns::Name)->text() == category)
            return true;

    return false;
}

void MainWindow::removeCategoryAndItsEntries(QModelIndex categoryIndex)
{
    tableResetFilter();

    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
    {
        QModelIndex realProcessIndex = processFilterProxyModel->sourceModel()->index(row, ProcessColumns::HiddenCategories);
        addOrRemoveProcessCategory(realProcessIndex, categoriesTableModel->item(categoryIndex.row(), CategoryColumns::Name)->text(), true);
    }

    categoriesTableModel->removeRows(categoryIndex.row(), 1);
    normalizeCategoryNumbers();
}

void MainWindow::normalizeCategoryNumbers()
{
    QList<QModelIndex> realIndexNumbers;
    for (int row = 0; row < categoriesTableModel->rowCount(); row++)
        realIndexNumbers.append(categoriesTableModel->index(row, CategoryColumns::HiddenNumber));

    std::sort(realIndexNumbers.begin(), realIndexNumbers.end(), MainWindow::compareQModelIndexData);

    for (int row = 0; row < categoriesTableModel->rowCount(); row++)
        categoriesTableModel->setData(realIndexNumbers.at(row), row + 1);
}

void MainWindow::addAllSelectedProcessesToCategory(QList<QModelIndex> realSelectedRows, QString category)
{
    foreach (QModelIndex index, realSelectedRows)
        if (!isProcessInCategory(index, category))
            addOrRemoveProcessCategory(index, category, false);
}

void MainWindow::removeAllCategoriesFromSelectedProcesses(QList<QModelIndex> realSelectedRows)
{
    if (getConfirmDialogAnswer("Remove all categories", "Are you sure you wish to remove all categories from the selected processes?") == QDialogButtonBox::Yes)
    {
        foreach (QModelIndex index, realSelectedRows)
            processFilterProxyModel->sourceModel()->setData(getRealIndex(index.row(), ProcessColumns::HiddenCategories), "");
    }
}

bool MainWindow::isProcessInCategory(QModelIndex realProcessIndex, QString category)
{
    QStringList processCategories = getRealIndexData(realProcessIndex.row(), ProcessColumns::HiddenCategories).toString().split(categoryDelimiter);
    return processCategories.contains(category);
}

void MainWindow::addOrRemoveProcessCategory(QModelIndex realProcessIndex, QString category, bool alreadyInCategory)
{
    QStringList processCategories = getRealIndexData(realProcessIndex.row(), ProcessColumns::HiddenCategories).toString().split(categoryDelimiter);
    processCategories.removeAll("");

    if (alreadyInCategory)
        processCategories.removeAll(category);
    else
        processCategories.append(category);

    processFilterProxyModel->sourceModel()->setData(getRealIndex(realProcessIndex.row(), ProcessColumns::HiddenCategories), processCategories.join(categoryDelimiter));
}

void MainWindow::renameCategory(QModelIndex categoryIndex, QString newName)
{
    QString oldName = categoriesTableModel->item(categoryIndex.row(), CategoryColumns::Name)->text();
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
    {
        QModelIndex realProcessIndex = getRealIndex(row, ProcessColumns::HiddenCategories);
        QStringList processCategories = realProcessIndex.data().toString().split(categoryDelimiter);
        QStringList renamedProcessCategories;

        foreach (QString processCategory, processCategories)
        {
            if (processCategory == oldName)
                renamedProcessCategories.append(newName);
            else
                renamedProcessCategories.append(processCategory);
        }

        processFilterProxyModel->sourceModel()->setData(realProcessIndex, renamedProcessCategories.join(categoryDelimiter));
    }

    categoriesTableModel->setData(categoriesTableModel->index(categoryIndex.row(), CategoryColumns::Name), newName);
}

void MainWindow::restoreTableFilterState(int lastCategoryRow)
{
    if (lastCategoryRow < 0 || lastCategoryRow >= categoriesTableModel->rowCount())
        tableResetFilter();
    else
        tableFilterByCategory(categoriesTableModel->index(lastCategoryRow, CategoryColumns::Name));
}

void MainWindow::tableResetFilter()
{
    ui->categoriesTable->clearSelection();
    currentlySelectedCategoriesRow = -1;
    processFilterProxyModel->setFilterFixedString("");
    processFilterProxyModel->setFilterKeyColumn(-1); // -1 means all columns
}

void MainWindow::tableFilterByCategory(QModelIndex categoryIndex)
{
    currentlySelectedCategoriesRow = categoryIndex.row();
    ui->categoriesTable->selectionModel()->select(categoriesTableModel->index(categoryIndex.row(), CategoryColumns::Name), QItemSelectionModel::Select);
    processFilterProxyModel->setFilterFixedString(categoriesTableModel->item(categoryIndex.row(), CategoryColumns::Name)->text());
    processFilterProxyModel->setFilterKeyColumn(ProcessColumns::HiddenCategories);
}

void MainWindow::tableFilterByText(QString text)
{
    processFilterProxyModel->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
    processFilterProxyModel->setFilterKeyColumn(FilterProcessColumns::NameAndNotes);
}

void MainWindow::moveSelectedRowsUp(int count)
{
    QList<QModelIndex> proxySelectedRows = ui->tableView->selectionModel()->selectedRows();
    if (proxySelectedRows.size() < 1 || proxySelectedRows.size() == processFilterProxyModel->sourceModel()->rowCount())
        return;

    QList<QModelIndex> realSelectedRowsNumbers = getRealIndexList(proxySelectedRows, ProcessColumns::Number);

    std::sort(realSelectedRowsNumbers.begin(), realSelectedRowsNumbers.end(), MainWindow::compareQModelIndexData);

    if (realSelectedRowsNumbers.first().data().toUInt() == 1)
        return;

    QList<QModelIndex> allRowNumbers;
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        allRowNumbers.append(getRealIndex(row, ProcessColumns::Number));

    std::sort(allRowNumbers.begin(), allRowNumbers.end(), MainWindow::compareQModelIndexData);

    while (count > 0)
    {
        for (int i = 0; i < realSelectedRowsNumbers.size(); i++)
        {
            QModelIndex upper = allRowNumbers[realSelectedRowsNumbers[i].data().toUInt() - 2];
            processFilterProxyModel->sourceModel()->setData(realSelectedRowsNumbers[i], realSelectedRowsNumbers[i].data().toUInt() - 1);
            processFilterProxyModel->sourceModel()->setData(upper, upper.data().toUInt() + 1);
            std::sort(allRowNumbers.begin(), allRowNumbers.end(), MainWindow::compareQModelIndexData);
        }
        count--;
    }
}

void MainWindow::moveSelectedRowsDown(int count)
{
    QList<QModelIndex> proxySelectedRows = ui->tableView->selectionModel()->selectedRows();
    if (proxySelectedRows.size() < 1 || proxySelectedRows.size() == processFilterProxyModel->sourceModel()->rowCount())
        return;

    QList<QModelIndex> realSelectedRowsNumbers = getRealIndexList(proxySelectedRows, ProcessColumns::Number);

    std::sort(realSelectedRowsNumbers.begin(), realSelectedRowsNumbers.end(), MainWindow::compareQModelIndexData);

    if (realSelectedRowsNumbers.last().data().toUInt() >= processFilterProxyModel->sourceModel()->rowCount())
        return;

    QList<QModelIndex> allRowNumbers;
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        allRowNumbers.append(getRealIndex(row, ProcessColumns::Number));

    std::sort(allRowNumbers.begin(), allRowNumbers.end(), MainWindow::compareQModelIndexData);

    while (count > 0)
    {
        for (int i = realSelectedRowsNumbers.size() - 1; i >= 0 ; i--)
        {
            QModelIndex lower = allRowNumbers[realSelectedRowsNumbers[i].data().toUInt()];
            processFilterProxyModel->sourceModel()->setData(realSelectedRowsNumbers[i], realSelectedRowsNumbers[i].data().toUInt() + 1);
            processFilterProxyModel->sourceModel()->setData(lower, lower.data().toUInt() - 1);
            std::sort(allRowNumbers.begin(), allRowNumbers.end(), MainWindow::compareQModelIndexData);
        }
        count--;
    }
}

QList<QModelIndex> MainWindow::getRealIndexList(QList<QModelIndex> proxyIndexList, int column)
{
    QList<QModelIndex> realIndexList;
    foreach (QModelIndex proxyIndex, proxyIndexList)
        realIndexList.append(processFilterProxyModel->mapToSource(getIndex(proxyIndex.row(), column)));
    return realIndexList;
}

QList<QModelIndex> MainWindow::getRealVisibleRows()
{
    QList<QModelIndex> proxyVisibleRows;
    for (int row = 0; row < processFilterProxyModel->rowCount(); row++)
        proxyVisibleRows.append(processFilterProxyModel->index(row, ProcessColumns::Name));
    return getRealIndexList(proxyVisibleRows, ProcessColumns::Name);
}

/*---------------------------------------------------- User input ----------------------------------------------------*/

void MainWindow::on_tableView_doubleClicked(const QModelIndex &proxyIndex)
{
    QString processName = getIndexData(proxyIndex.row(), ProcessColumns::Name).toString();

    switch (proxyIndex.column())
    {
        case ProcessColumns::Tracking:
        {
            setProcessesPaused(QList<QModelIndex>() << processFilterProxyModel->mapToSource(proxyIndex), getIndexData(proxyIndex.row(), ProcessColumns::Tracking).toString() == processIsActiveSymbol);
            break;
        }

        case ProcessColumns::Icon:
        {
            QString iconPath = QFileDialog::getOpenFileName(this, "Open Image", "", Utility::imageFormats);

            if (iconPath.isEmpty() && processIcons[processName] != defaultProcessIcon)
                return;

            QIcon icon = getIcon(processName, iconPath);

            if (!iconPath.isEmpty() && !icon.isNull())
                processFilterProxyModel->setData(getIndex(proxyIndex.row(), ProcessColumns::Icon), icon, Qt::DecorationRole);

            break;
        }

        case ProcessColumns::Name:
        {
            removeSelectedRows(QList<QModelIndex>() << proxyIndex);
            break;
        }

        case ProcessColumns::Duration:
        {
            bool ok;
            QString displayDuration = getIndexData(proxyIndex.row(), ProcessColumns::Duration).toString();
            QString durationInput = QInputDialog::getText(this, "Custom duration", "Set custom duration", QLineEdit::Normal, displayDuration, &ok);

            if (ok)
                processDurations[processName] = Parser::parseStringToDuration(durationInput);

            break;
        }
    }
}

void MainWindow::tableCellCustomContextMenuRequested(const QPoint &pos)
{
    QList<QModelIndex> proxySelectedRows = ui->tableView->selectionModel()->selectedRows();
    if (proxySelectedRows.size() == 0)
        return;

    QMenu *menu = new QMenu(this);

    // Pause action
    QAction *action = new QAction("Pause", this);
    connect(action, &QAction::triggered, this, [=](){setProcessesPaused(getRealIndexList(proxySelectedRows, ProcessColumns::Tracking), true);});
    menu->addAction(action);

    // Resume action
    action = new QAction("Resume", this);
    connect(action, &QAction::triggered, this, [=](){setProcessesPaused(getRealIndexList(proxySelectedRows, ProcessColumns::Tracking), false);});
    menu->addAction(action);

    if (proxySelectedRows.size() == 1)
    {
        action = new QAction("Change icon", this);
        connect(action, &QAction::triggered, this, [=](){on_tableView_doubleClicked(processFilterProxyModel->index(proxySelectedRows.first().row(), ProcessColumns::Icon));});
        menu->addAction(action);

        action = new QAction("Change duration", this);
        connect(action, &QAction::triggered, this, [=](){on_tableView_doubleClicked(processFilterProxyModel->index(proxySelectedRows.first().row(), ProcessColumns::Duration));});
        menu->addAction(action);
    }

    // Category management actions
    // On multiple selected - Add all to specific category or remove all categories
    // On single selected - Add to specific category or remove from specific category via checked actions
    QMenu *categoriesSubMenu = menu->addMenu("Categories");
    if (proxySelectedRows.size() > 1)
    {
        // Categories menu for multiple selected
        QMenu *addAllSubMenu = categoriesSubMenu->addMenu("Add all to...");
        foreach (QString category, getDelimitedCategories())
        {
            action = new QAction(category, this);
            connect(action, &QAction::triggered, this, [=](){addAllSelectedProcessesToCategory(getRealIndexList(proxySelectedRows, ProcessColumns::HiddenCategories), category);});
            addAllSubMenu->addAction(action);
        }

        action = new QAction("Remove all", this);
        connect(action, &QAction::triggered, this, [=](){removeAllCategoriesFromSelectedProcesses(getRealIndexList(proxySelectedRows, ProcessColumns::HiddenCategories));});
        categoriesSubMenu->addAction(action);
    }
    else
    {
        // Categories menu for single selected
        QModelIndex realIndex = processFilterProxyModel->mapToSource(proxySelectedRows.first());

        foreach (QString category, getDelimitedCategories())
        {
            action = new QAction(category, this);
            action->setCheckable(true);
            action->setChecked(isProcessInCategory(realIndex, category));
            connect(action, &QAction::triggered, this, [=](bool checked){addOrRemoveProcessCategory(realIndex, category, !checked);});
            categoriesSubMenu->addAction(action);
        }
    }

    // Remove action
    action = new QAction("Remove", this);
    connect(action, &QAction::triggered, this, [=](){removeSelectedRows(proxySelectedRows);});
    menu->addAction(action);

    // Export action
    action = new QAction("Export", this);
    connect(action, &QAction::triggered, this, [=](){exportProcesses(getRealIndexList(proxySelectedRows, ProcessColumns::Name));});
    menu->addAction(action);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::tableHorizontalHeaderCustomContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    for (int column = ProcessColumns::Number; column < processFilterProxyModel->sourceModel()->columnCount(); column++)
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

void MainWindow::categoriesTableCustomContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    QAction *action = new QAction("Add category", this);
    connect(action, &QAction::triggered, this, [=]()
    {
        QString category = QInputDialog::getText(this, "Add category", "Set category name", QLineEdit::Normal, "");
        category.remove(categoryDelimiter);
        if (!category.isEmpty())
        {
            if (!categoryAlreadyExists(category))
                createCategoryInTable(categoriesTableModel->rowCount() + 1, category);
            else
                systemTrayIcon->showMessage("Error", "Category " + category + " already exists", QSystemTrayIcon::Warning, 3000);
        }
    });
    menu->addAction(action);

    QModelIndex selectedCategory = ui->categoriesTable->indexAt(pos);
    if (selectedCategory.row() >= 0 && selectedCategory.column() == CategoryColumns::Name)
    {
        action = new QAction("Rename category", this);
        connect(action, &QAction::triggered, this, [=]()
        {
            QString newName = QInputDialog::getText(this, "Rename category", "New category name", QLineEdit::Normal, "");
            newName.remove(categoryDelimiter);
            if (!newName.isEmpty())
            {
                if (!categoryAlreadyExists(newName))
                    renameCategory(selectedCategory, newName);
                else
                    systemTrayIcon->showMessage("Error", "Category " + newName + " already exists", QSystemTrayIcon::Warning, 3000);
            }
        });
        menu->addAction(action);

        action = new QAction("Remove category", this);
        connect(action, &QAction::triggered, this, [=]()
        {
            QString category = categoriesTableModel->item(selectedCategory.row(), CategoryColumns::Name)->text();

            if (getConfirmDialogAnswer("Remove category", "Are you sure you wish to remove category "
                + category + "? This action is irreversible!") == QMessageBox::Yes)
                removeCategoryAndItsEntries(selectedCategory);
        });
        menu->addAction(action);
    }

    menu->popup(ui->categoriesTable->viewport()->mapToGlobal(pos));
}

void MainWindow::on_categoriesTable_clicked(const QModelIndex &index)
{
    if (currentlySelectedCategoriesRow == index.row())
        tableResetFilter();
    else
        tableFilterByCategory(index);
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

void MainWindow::on_actionMove_to_Top_triggered()
{
    QList<QModelIndex> proxySelectedRows = ui->tableView->selectionModel()->selectedRows();
     if (proxySelectedRows.size() < 1 || proxySelectedRows.size() == processFilterProxyModel->sourceModel()->rowCount())
         return;

     QList<QModelIndex> proxySelectedRowsNumbers;
     foreach (QModelIndex index, proxySelectedRows)
         proxySelectedRowsNumbers.append(getIndex(index.row(), ProcessColumns::Number));

    std::sort(proxySelectedRowsNumbers.begin(), proxySelectedRowsNumbers.end(), MainWindow::compareQModelIndexData);

    moveSelectedRowsUp(getIndexData(proxySelectedRowsNumbers.first().row(), ProcessColumns::Number).toUInt() - 1);
}

void MainWindow::on_actionMove_Up_triggered()
{
    moveSelectedRowsUp();
}

void MainWindow::on_actionMove_Down_triggered()
{
    moveSelectedRowsDown();
}

void MainWindow::on_actionMove_to_Bottom_triggered()
{
    QList<QModelIndex> proxySelectedRows = ui->tableView->selectionModel()->selectedRows();
     if (proxySelectedRows.size() < 1 || proxySelectedRows.size() == processFilterProxyModel->sourceModel()->rowCount())
         return;

     QList<QModelIndex> proxySelectedRowsNumbers;
     foreach (QModelIndex index, proxySelectedRows)
         proxySelectedRowsNumbers.append(getIndex(index.row(), ProcessColumns::Number));

    std::sort(proxySelectedRowsNumbers.begin(), proxySelectedRowsNumbers.end(), MainWindow::compareQModelIndexData);

    moveSelectedRowsDown(processFilterProxyModel->sourceModel()->rowCount()
        - getIndexData(proxySelectedRowsNumbers.last().row(), ProcessColumns::Number).toUInt());
}

void MainWindow::on_actionOptions_triggered()
{
    Options *options = new Options();
    connect(options, &Options::userOptionsChosen, this, &MainWindow::userOptionsChosen);
    options->show();
}

void MainWindow::on_actionExport_triggered()
{
    QList<QModelIndex> realProcesses;
    for (int row = 0; row < processFilterProxyModel->sourceModel()->rowCount(); row++)
        realProcesses.append(getRealIndex(row, ProcessColumns::Name));
    exportProcesses(realProcesses);
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

            QString categories = processData["categories"].toString();
            foreach (QString category, categories.split(categoryDelimiter))
            {
                if (!categoryAlreadyExists(category))
                {
                    createCategoryInTable(categoriesTableModel->rowCount() + 1, category);
                    systemTrayIcon->showMessage("Import created missing category", category, QSystemTrayIcon::Information, 3000);
                }
            }

            createProcessInTable(
                categories,
                QString::number(processFilterProxyModel->sourceModel()->rowCount() + 1),
                processData["tracking"].toBool() ? processIsActiveSymbol : processIsPausedSymbol,
                getIcon(processName, processData["iconPath"].toString()),
                processName,
                processData["duration"].toVariant().toULongLong(),
                processData["notes"].toString(),
                processData["lastSeen"].toString(),
                processData["dateAdded"].toString());
        }
    }
}

void MainWindow::on_actionStretch_triggered()
{
    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
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
        if (currentlySelectedCategoriesRow == -1)
            tableResetFilter();
        else
            tableFilterByCategory(categoriesTableModel->index(currentlySelectedCategoriesRow, CategoryColumns::Name));
    }
    else
        tableFilterByText(arg1);
}

void MainWindow::on_moveCategoryUpButton_clicked()
{
    if (currentlySelectedCategoriesRow <= 0)
        return;

    QModelIndex selectedCategoryNumber = categoriesTableModel->index(currentlySelectedCategoriesRow--, CategoryColumns::HiddenNumber);

    QModelIndex upper = categoriesTableModel->index(selectedCategoryNumber.row() - 1, CategoryColumns::HiddenNumber);
    QVariant tempNumber = categoriesTableModel->data(selectedCategoryNumber);
    categoriesTableModel->setData(selectedCategoryNumber, upper.data());
    categoriesTableModel->setData(upper, tempNumber);
    ui->categoriesTable->sortByColumn(CategoryColumns::HiddenNumber, Qt::AscendingOrder);
}

void MainWindow::on_moveCategoryDownButton_clicked()
{
    if (currentlySelectedCategoriesRow < 0 || currentlySelectedCategoriesRow >= categoriesTableModel->rowCount() - 1)
        return;

    QModelIndex selectedCategoryNumber = categoriesTableModel->index(currentlySelectedCategoriesRow++, CategoryColumns::HiddenNumber);

    QModelIndex lower = categoriesTableModel->index(selectedCategoryNumber.row() + 1, CategoryColumns::HiddenNumber);
    QVariant tempNumber = categoriesTableModel->data(selectedCategoryNumber);
    categoriesTableModel->setData(selectedCategoryNumber, lower.data());
    categoriesTableModel->setData(lower, tempNumber);
    ui->categoriesTable->sortByColumn(CategoryColumns::HiddenNumber, Qt::AscendingOrder);
}

void MainWindow::systemTrayIconActionOpen()
{
    this->show();
}

void MainWindow::systemTrayIconActionResumeAll()
{
      setProcessesPaused(getRealVisibleRows(), false);
}

void MainWindow::systemTrayIconActionPauseAll()
{
    setProcessesPaused(getRealVisibleRows(), true);
}

void MainWindow::systemTrayIconActionExit()
{
    qApp->quit();
}
