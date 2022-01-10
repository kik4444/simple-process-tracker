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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr, uint pollInterval = 5);
    ~Options();

signals:
    void userOptionsChosen(uint processPollInterval);

private slots:
    void on_confirmBox_rejected();

    void on_confirmBox_accepted();

private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
