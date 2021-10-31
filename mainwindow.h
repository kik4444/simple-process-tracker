#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>

//For additional components or UIs
#include "ui/trackentry/trackentry.h"
#include "ui/processDialog/processdialog.h"
#include "ui/options/options.h"

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

private:
    Ui::MainWindow *ui;

    void configureTrackEntry(TrackEntry *trackEntry);

    QSystemTrayIcon *systemTrayIcon;

    const int trackEntrySize = 50;
    const int autoSaveTimerInterval = 60000;
};
#endif // MAINWINDOW_H
