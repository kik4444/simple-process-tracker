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

// For quick access to QSettings
#define quicksettings(location) QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", location)

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

    void createProcessInTable(QString activeSymbol, QIcon icon, QString processName, QString notes, quint64 duration, QString lastSeen, QString dateAdded);
    QIcon getIcon(QString processName, QString iconPath);

    void loadProcessData();
    void loadWindowData();
    void saveWindowData();

    int getConfirmDialogAnswer(QString title, QString text);

    void updateLastSeenIfRunningAndRemove(QString processName, int row);
    void updateLastSeenForRunningProcesses();

private: // Variables
    Ui::MainWindow *ui;

    MyStandardItemModel *processTableViewModel = new MyStandardItemModel();

    QSystemTrayIcon *systemTrayIcon;

    QTimer *processPollTimer, *runningProcessDurationsUpdateTimer, *processDataAutoSaveTimer;

    QMap<QString, quint64> processDurations;
    QStringList runningProcesses;
    QMap<QString, QString> processIcons;

    uint processPollInterval = 5000;

    const QString processIsActiveSymbol = "▶";
    const QString processIsPausedSymbol = "⏸";

    const uint processSecondIncrementInterval = 1000;
    const uint processAutoSaveInterval = 60000;

signals:
    void checkRunningProcesses(QMap<QString, int> processList);

public slots:
    void newProcessAdded(QString processName, QString iconPath);
    void foundRunningProcess(QString processName);
    void foundStoppedProcesses(QMap<QString, int> stoppedProcesses);

    void userOptionsChosen(uint processPollInterval);

private slots:
    void on_actionDebug_triggered();

    void saveProcessData();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void tableCellCustomContextMenuRequested(const QPoint &pos);
    void tableHorizontalHeaderCustomContextMenuRequested(const QPoint &pos);
    void tableVerticalHeaderCustomContextMenuRequested(const QPoint &pos);

    void on_actionAdd_triggered();
    void on_actionPoll_triggered();
    void on_actionInvert_hidden_triggered();
    void on_actionStretch_triggered();
    void on_actionOptions_triggered();
    void on_actionExit_triggered();

    void systemTrayIconActionOpen();
    void systemTrayIconActionResumeAll();
    void systemTrayIconActionPauseAll();
    void systemTrayIconActionShowAll();
    void systemTrayIconActionHideAll();
    void systemTrayIconActionExit();
    void on_actionHelp_triggered();
};
#endif // MAINWINDOW_H
