#ifndef TRACKENTRY_H
#define TRACKENTRY_H

#include <QWidget>

//For updating duration
#include <QTimer>

//For removal confirmation
#include <QMessageBox>

//For inputting custom duration
#include <QInputDialog>

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
    uint getProcessDuration();
    QString getDateAdded();
    bool getTrackingIsActive();

    void setData(QString processName, uint processDuration, QString dateAdded, bool trackingIsActive);

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

    uint processDuration = 0;
    bool trackingIsActive = false;

    QTimer *timer;
    const uint timerInterval = 1000;
    uint updateInterval = timerInterval / 1000;
};

#endif // TRACKENTRY_H
