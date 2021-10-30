#ifndef TRACKENTRY_H
#define TRACKENTRY_H

#include <QWidget>

//For updating duration
#include <QTimer>

//For removal confirmation
#include <QMessageBox>

//For inputting custom duration
#include <QInputDialog>

//For selecting an icon
#include <QFileDialog>

//For icons
#include <QPixmap>

//For Windows API to check process status
#ifdef Q_OS_WINDOWS

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#endif

namespace Ui {
class TrackEntry;
}

class TrackEntry : public QWidget
{
    Q_OBJECT

public:
    explicit TrackEntry(QWidget *parent = nullptr);
    ~TrackEntry();
    Ui::TrackEntry *ui;

    QString getProcessName();
    QString getIconPath();
    uint getProcessDuration();
    QString getDateAdded();
    bool getTrackingIsActive();

    void setData(QString processName, QString iconPath, uint processDuration, QString dateAdded, bool trackingIsActive);

signals:
    void removeClearedEntries();

public slots:
    void processChosen(QString processName);

private slots:
    void updateDuration();

    void on_selectButton_clicked();

    void on_trackingCheckBox_stateChanged(int arg1);

    void on_removeButton_clicked();

    void on_durationButton_clicked();

private:
    QString parseProcessDuration(uint processDuration);
    void setTimerState();
    bool eventFilter(QObject *object, QEvent *event);

    #ifdef Q_OS_WINDOWS
    bool IsProcessRunning(const TCHAR* const executableName);
    #endif

    uint processDuration = 0;
    bool trackingIsActive = false;
    QString iconPath = ":/Assets/Icons/app-icon.svg";

    QTimer *timer;
    const uint timerInterval = 1000;
    uint updateInterval = timerInterval / 1000;

    const uint iconSize = 35;
};

#endif // TRACKENTRY_H
