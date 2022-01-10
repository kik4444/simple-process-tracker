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

// For showing list of processes
#include "processdialog.h"

// For scanning for processes
#include "processscanner.h"

// For parsing durations
#include "parser.h"

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
    void saveWindowData();

private: // Variables
    Ui::MainWindow *ui;

    MyStandardItemModel *processTableViewModel = new MyStandardItemModel();

    QMap<QString, quint64> processDurations;
    QStringList runningProcesses;
    QMap<QString, QString> processIcons;

    const QString processIsActiveSymbol = "▶";
    const QString processIsPausedSymbol = "⏸";
//    const QMap<bool, QString> processTrackingSymbol = {{true, "▶"}, {false, "⏸"}};

    const uint processAutoSaveInterval = 60000;
    //TODO replace me with user-defined
    const uint processPollInterval = 5000;

signals:
    void checkRunningProcesses(QMap<QString, int> processList);

public slots:
    void newProcessAdded(QString processName, QString iconPath);
    void foundRunningProcess(QString processName);
    void foundStoppedProcesses(QMap<QString, int> stoppedProcesses);

private slots:
    void saveProcessData();

    void on_actionAdd_triggered();
    void on_actionDebug_triggered();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_actionPoll_triggered();
    void on_actionExit_triggered();
};
#endif // MAINWINDOW_H
