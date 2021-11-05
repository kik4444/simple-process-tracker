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

//For additional components or UIs
#include "ui/trackentry/trackentry.h"
#include "ui/processDialog/processdialog.h"
#include "ui/options/options.h"
#include "ui/about/about.h"

//For reading process list
#include <QProcess>

//For saving / loading processes
#include <QSettings>

//For reordering processes
#include <QMap>

//For running in the background
#include <QSystemTrayIcon>

//For recording date added
#include <QDateTime>

//For platform-specific actions
#include "custom/platform_specifics.h"

//For saving user options
#include <QVariantMap>

//For SVG icon
#include <QtSvg>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static QVariantMap userSettings;

signals:
    void forcePollProcesses();

public slots:
    void removeClearedEntries();

    void saveProcessData();

    void removeHiddenProcess(QString processName);

private slots:
    void trayIconActionOpen();
    void trayIconActivated(int activationReason);
    void trayIconActionExit();

    void on_actionAdd_triggered();

    void on_actionExit_triggered();

    void on_actionOptions_triggered();

    void on_actionPoll_triggered();

    void on_actionShow_hidden_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    void loadProcessData(bool showHidden = false);

    void configureTrackEntry(TrackEntry *trackEntry);

    QSystemTrayIcon *systemTrayIcon;

    bool showHidden = false;

    const int trackEntrySize = 50;
    const int autoSaveTimerInterval = 60000;
};
#endif // MAINWINDOW_H
