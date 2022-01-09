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
    bool getHidden();

    void setData(QString processName, QString iconPath, uint processDuration, QString dateAdded, bool trackingIsActive, bool hidden);

signals:
    void removeClearedEntries();

    void saveProcessData();

    void removeHiddenProcess(QString processName);

public slots:
    void processChosen(QString processName);

    void pollProcess();

    void on_trackingCheckBox_stateChanged(int arg1);

    void updateProcessLastSeenIfRunning();

private slots:
    void updateDuration();

    void on_selectButton_clicked();

    void on_removeButton_clicked();

    void on_durationButton_clicked();

    void on_hideButton_clicked();

private:
    QString parseProcessDuration(uint processDuration);

    void setTimerState();

    bool eventFilter(QObject *object, QEvent *event);

    void setHideChanges();

    void setLastSeenNow();

    QString iconPath = ":/Assets/Icons/app-icon.svg";
    uint processDuration = 0;
    bool trackingIsActive = false;
    bool hidden = false;

    QTimer *updateTimer;
    const uint updateTimerInterval = 1000;
    uint updateInterval = updateTimerInterval / 1000;

    QTimer *processPollTimer;

    const uint iconSize = 35;
};

#endif // TRACKENTRY_H
