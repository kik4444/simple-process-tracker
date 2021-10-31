#ifndef PLATFORM_SPECIFICS_H
#define PLATFORM_SPECIFICS_H
#include "../mainwindow.h"

//For checking if process is running on Linux
#if defined Q_OS_LINUX

#include <QDir>

//For Windows API to check if process is running
#elif defined Q_OS_WINDOWS

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#endif

class Platform
{
public:
    static QStringList getProcessList(ProcessDialog *thiz)
    {
        QStringList processList;
        QProcess *qprocess = new QProcess(thiz);

        #if defined Q_OS_LINUX

        qprocess->start("ps", QStringList() << "-eo" << "comm");
        qprocess->waitForFinished();
        processList = QString(qprocess->readAllStandardOutput()).split("\n");

        #elif defined Q_OS_MACOS

        //mac stuff

        #elif defined Q_OS_WINDOWS

        qprocess->start("cmd.exe", QStringList() << "/k");
        qprocess->write("for /f \"tokens=1 delims=,\" %F in ('tasklist /nh /fo csv') do @echo %~F\n\rexit\n\r");
        qprocess->waitForFinished();
        processList = QString(qprocess->readAllStandardOutput()).split("\n");

        #endif

        qprocess->deleteLater();

        processList.removeDuplicates();
        processList.removeAll(QString(""));
        processList.sort(Qt::CaseInsensitive);
        return processList;
    }

    static bool isProcessRunning(QString processName)
    {
        #if defined Q_OS_LINUX

        foreach (QString pid, QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
        {
            QFile file("/proc/" + pid + "/comm");
            if (file.open(QIODevice::ReadOnly))
            {
                if (QString::compare(file.readAll(), processName + "\n") == 0)
                    return true;
            }
        }

        #elif defined Q_OS_MACOS

        //mac stuff

        #elif defined Q_OS_WINDOWS
        //Credit - https://stackoverflow.com/a/57164620

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (!Process32First(snapshot, &entry))
        {
            CloseHandle(snapshot);
            return false;
        }

        do
        {
            if (!_tcsicmp(entry.szExeFile, processName.toStdWString().c_str()))
            {
                CloseHandle(snapshot);
                return true;
            }
        }
        while (Process32Next(snapshot, &entry));

        CloseHandle(snapshot);

        #endif

        return false;
    }
};

#endif // PLATFORM_SPECIFICS_H