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

#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) : QWidget(parent), ui(new Ui::Options)
{
    ui->setupUi(this);
    quicksettings("config");
    ui->processPollIntervalSpinBox->setValue(settings.value("processPollInterval", 5000).toUInt() / 1000);
    ui->startInBackgroundCheckBox->setChecked(settings.value("startInBackground").toBool());
}

Options::~Options()
{
    delete ui;
}

void Options::on_confirmBox_rejected()
{
    this->~Options();
}

void Options::on_confirmBox_accepted()
{
    quicksettings("config");

    uint processPollInterval = ui->processPollIntervalSpinBox->value() * 1000;

    settings.setValue("processPollInterval", processPollInterval);
    settings.setValue("startInBackground", ui->startInBackgroundCheckBox->isChecked());

    emit userOptionsChosen(processPollInterval);

    this->~Options();
}
