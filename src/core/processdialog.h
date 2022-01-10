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

#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);
    ~ProcessDialog();

signals:
    void newProcessAdded(QString processName, QString iconPath);

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_filterLineEdit_textChanged(const QString &arg1);

    void on_iconBrowseButton_clicked();

private:
    Ui::ProcessDialog *ui;

    QStringList processList;
};

#endif // PROCESSDIALOG_H
