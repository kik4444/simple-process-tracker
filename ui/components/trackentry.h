#ifndef TRACKENTRY_H
#define TRACKENTRY_H

#include <QWidget>

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

    void setData(QString processName, uint processDuration);

public slots:
    void processChosen(QString processName);

private slots:
    void on_selectButton_clicked();

private:
    QString parseProcessDuration(uint processDuration);
};

#endif // TRACKENTRY_H
