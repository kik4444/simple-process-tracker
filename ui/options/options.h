#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>
#include "../../mainwindow.h"

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr);
    ~Options();

private slots:
    void on_confirmBox_rejected();

    void on_confirmBox_accepted();

private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
