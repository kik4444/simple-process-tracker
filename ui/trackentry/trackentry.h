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

public slots:
    void processChosen(QString processName);

    void pollProcess();

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
