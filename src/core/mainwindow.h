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

// For icons in the table view
#include "../subclass/myitemdelegate.h"

// For showing list of processes
#include "processdialog.h"

// For scanning for processes
#include "processscanner.h"

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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

namespace ProcessColumns
{
    enum ProcessColumns
    {
        Icon = 0,
        Name = 1,
        Duration = 2,
        DateAdded = 3,
        LastSeen = 4
    };
}

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

private: // Variables
    Ui::MainWindow *ui;

    QStandardItemModel *processTableViewModel = new QStandardItemModel();

    QMap<QString, qint64> processDurations;

signals:

public slots:
    void processChosen(QString processName, QString iconPath);

private slots:
    void on_tableView_clicked(const QModelIndex &index);
    void on_actionAdd_triggered();
    void on_actionDebug_triggered();
};
#endif // MAINWINDOW_H
