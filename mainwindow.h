#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>

//For additional components or UIs
#include "ui/components/trackentry.h"
#include "ui/processDialog/processdialog.h"

//For reading process list
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_triggered();

private:
    Ui::MainWindow *ui;

    const int trackEntrySize = 50;
};
#endif // MAINWINDOW_H
