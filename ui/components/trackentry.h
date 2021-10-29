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

    void setText(QString buttonText, QString lineText);

private:
};

#endif // TRACKENTRY_H
