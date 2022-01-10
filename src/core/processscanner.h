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

#ifndef PROCESSSCANNER_H
#define PROCESSSCANNER_H

#include "mainwindow.h"

// Check for processes on Linux
#if defined Q_OS_LINUX

#include <QDir>

// Check for processes on Windows
#elif defined Q_OS_WINDOWS

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#endif

class ProcessScanner : public QWidget
{
    Q_OBJECT

public:
    ProcessScanner();

    static QStringList getProcessList(ProcessDialog *thiz);

signals:
    void foundRunningProcess(QString processName);
    void foundStoppedProcesses(QStringList stoppedProcesses);

public slots:
    void checkRunningProcesses(QStringList processList);
};

#endif // PROCESSSCANNER_H
