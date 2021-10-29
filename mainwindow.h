#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>

//For additional components or UIs
#include "ui/components/trackentry.h"
#include "ui/processDialog/processdialog.h"

//For reading process list
#include <QProcess>

//For saving / loading processes
#include <QSettings>

//For reordering processes
#include <QMap>

//For running in the background
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void removeClearedEntries();

private slots:
    void trayIconActionOpen();
    void trayIconActivated(int activationReason);
    void trayIconActionExit();

    void on_actionAdd_triggered();

    void on_actionDebug_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *systemTrayIcon;

    const int trackEntrySize = 50;
};
#endif // MAINWINDOW_H
