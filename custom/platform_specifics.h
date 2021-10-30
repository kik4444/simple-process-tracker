#ifndef PLATFORM_SPECIFICS_H
#define PLATFORM_SPECIFICS_H
#include "../mainwindow.h"

class Platform
{
public:
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
