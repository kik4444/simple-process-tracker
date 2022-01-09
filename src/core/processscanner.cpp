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

#include "processscanner.h"

ProcessScanner::ProcessScanner(QMainWindow *thiz) {}

QStringList ProcessScanner::getProcessList(ProcessDialog *thiz)
{
    QStringList processList;
    QProcess *qprocess = new QProcess(thiz);

    #if defined Q_OS_LINUX

    qprocess->start("ps", QStringList() << "-eo" << "comm");
    qprocess->waitForFinished();
    processList = QString(qprocess->readAllStandardOutput()).split("\n");

    #elif defined Q_OS_MACOS

    qprocess->start("ps", QStringList() << "-eo" << "ucomm");
    qprocess->waitForFinished();
    processList = QString(qprocess->readAllStandardOutput()).split("\n");

    #elif defined Q_OS_WINDOWS

    qprocess->start("cmd.exe", QStringList() << "/k");
    qprocess->write("for /f \"tokens=1 delims=,\" %F in ('tasklist /nh /fo csv') do @echo %~F\n\rexit\n\r");
    qprocess->waitForFinished();
    processList = QString(qprocess->readAllStandardOutput()).split("\n");
    processList.removeAll(QString("\r"));

    #endif

    qprocess->deleteLater();

    processList.removeDuplicates();
    processList.removeAll(QString(""));
    processList.sort(Qt::CaseInsensitive);
    return processList;
}

void ProcessScanner::checkRunningProcesses(QStringList processList)
{

}
