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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>

// For icons in the table view and centered text
#include "../subclass/myitemdelegate.h"

// For setting specific columns as read-only or read-write
#include "../subclass/mystandarditemmodel.h"

// For natural duration sorting
#include "../subclass/mystandarditem.h"

// For easier QSortFilterProxyModel modifications
#include "../subclass/mysortfilterproxymodel.h"

// For showing list of processes
#include "processdialog.h"

// For scanning for processes
#include "processscanner.h"

// For user options
#include "options.h"

// For parsing durations
#include "parser.h"

// For opening the help section
#include "about.h"

// For reading process list
#include <QProcess>

// For saving / loading processes
#include <QSettings>

// For running in the background
#include <QSystemTrayIcon>

// For recording date added
#include <QDateTime>

// For saving user options
#include <QVariantMap>

// For SVG icons
#include <QtSvg>

// For exporting or importing Json backups
#include <QJsonDocument>

// For quick access to QSettings
#define quicksettings(location) QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", location)

// To easily switch to and from debug mode
//#define DEBUG_MODE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

namespace Utility
{
    const QString imageFormats = "Image Files (*.svg *.svgz *.png *.jpg *.jpeg *.webp *.bmp *.tiff *.tif)";
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public: // Methods
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private: // Methods
    void pollProcesses();
    void updateRunningProcessDurations();

    QIcon getIcon(QString processName, QString iconPath);
    void createProcessInTable(QString categories, QString number, QString activeSymbol, QIcon icon, QString processName, QString notes, quint64 duration, QString lastSeen, QString dateAdded);
    void createCategoryInTable(QString category);

    QModelIndex getIndex(int row, int column);
    QVariant getIndexData(int row, int column);

    QStringList getDelimitedCategories();
    void createCategoriesFromDelimitedList(QString delimitedCategories);

    void loadProcessData();
    void loadWindowData();
    void saveWindowData();

    int getConfirmDialogAnswer(QString title, QString text);

    bool processAlreadyExists(QString processName);

    void updateLastSeenIfRunningAndRemoveFromRunning(QString processName, int row);
    void updateLastSeenForRunningProcesses();

    void setProcessPaused(QModelIndex processIndex, bool paused);

    void removeSelectedRows(QList<QModelIndex> selectedRows);
    void normalizeProcessNumbers();

    void exportSelectedRows(QList<QModelIndex> selectedRows);
    bool isJsonValid(QJsonObject jsonObject);

    bool categoryAlreadyExists(QString category);
    void removeCategoryAndItsEntries(QModelIndex categoryIndex);
    void addAllSelectedProcessesToCategory(QList<QModelIndex> selectedProcesses, QString category);
    void removeAllCategoriesFromSelectedProcesses(QList<QModelIndex> selectedProcesses);
    bool processIsInCategory(QModelIndex processIndex, QString category);
    void addOrRemoveProcessCategory(QModelIndex processIndex, QString category, bool alreadyInCategory);
    void renameCategory(QModelIndex categoryIndex, QString newName);

    void restoreTableFilterState(int categorySelection);
    void tableResetFilter(QModelIndex categoryIndex = QModelIndex());
    void tableFilterByCategory(QModelIndex index);
    void tableFilterByText(QString text);

private: // Variables
    Ui::MainWindow *ui;

    MyStandardItemModel *processTableViewModel = new MyStandardItemModel();
    MySortFilterProxyModel *processFilterProxyModel;

    QStandardItemModel *categoriesTableModel = new QStandardItemModel();

    QSystemTrayIcon *systemTrayIcon;

    QTimer *processPollTimer, *runningProcessDurationsUpdateTimer, *processDataAutoSaveTimer;

    QMap<QString, quint64> processDurations;
    QStringList runningProcesses;
    QMap<QString, QString> processIcons;

    uint processPollInterval = 5000;

    int currentlySelectedCategoriesRow = -1;

    const QString processIsActiveSymbol = "▶";
    const QString processIsPausedSymbol = "⏸";

    const uint processSecondIncrementInterval = 1000;
    const uint processAutoSaveInterval = 60000;

    const QChar categoryDelimiter = ';';

signals:
    void checkRunningProcesses(QMap<QString, int> processList);

public slots:
    void newProcessAdded(QString processName, QString iconPath);
    void foundRunningProcess(QString processName);
    void foundStoppedProcesses(QMap<QString, int> stoppedProcesses);

    void userOptionsChosen(uint processPollInterval);

private slots:
    #ifdef DEBUG_MODE
    void on_actionDebug_triggered();
    #endif

    void saveProcessData();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void tableCellCustomContextMenuRequested(const QPoint &pos);
    void tableHorizontalHeaderCustomContextMenuRequested(const QPoint &pos);
    void categoriesTableCustomContextMenuRequested(const QPoint &pos);

    void on_categoriesTable_clicked(const QModelIndex &index);

    void on_actionAdd_triggered();
    void on_actionPoll_triggered();
    void on_actionMove_Up_triggered();
    void on_actionMove_Down_triggered();
    void on_actionOptions_triggered();
    void on_actionImport_triggered();
    void on_actionStretch_triggered();
    void on_actionExit_triggered();
    void processFilterLineEdit_textChanged(const QString &arg1);

    void systemTrayIconActionOpen();
    void systemTrayIconActionResumeAll();
    void systemTrayIconActionPauseAll();
    void systemTrayIconActionExit();
    void on_actionHelp_triggered();
};
#endif // MAINWINDOW_H
