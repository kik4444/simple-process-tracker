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

#include "mainwindow.h"
#include "processdialog.h"
#include "ui_processdialog.h"

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);

    processList = ProcessScanner::getProcessList(this);
    ui->listWidget->addItems(processList);
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

void ProcessDialog::on_buttonBox_rejected()
{
    this->~ProcessDialog();
}

void ProcessDialog::on_listWidget_itemActivated(QListWidgetItem *item)
{
    emit processChosen(item->text().remove("\n").remove("\r").trimmed(), ui->iconPathLineEdit->text());
    this->~ProcessDialog();
}

void ProcessDialog::on_buttonBox_accepted()
{
    on_listWidget_itemActivated(ui->listWidget->currentItem());
}

void ProcessDialog::on_filterLineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();

    if (!arg1.isEmpty())
    {
        QStringList results = processList.filter(arg1, Qt::CaseInsensitive);
        ui->listWidget->addItems(results);
    }
    else
        ui->listWidget->addItems(processList);
}


void ProcessDialog::on_iconBrowseButton_clicked()
{
    ui->iconPathLineEdit->setText(QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files " + Utility::imageFormats));
}
