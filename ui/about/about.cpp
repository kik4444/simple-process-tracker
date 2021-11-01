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

#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) : QWidget(parent), ui(new Ui::About)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::on_buttonBox_accepted()
{
    this->~About();
}

QString About::readInternalText(QString path)
{
    QFile text(path);
    return text.open(QIODevice::ReadOnly) ? text.readAll() : QString();
}

void About::on_licenseButton_clicked()
{
    ui->textEdit->setText(readInternalText(":/Assets/about-text/about.html"));
}

void About::on_qtButton_clicked()
{
    ui->textEdit->setText(readInternalText(":/Assets/about-text/qt.html"));
}

void About::on_aboutButton_clicked()
{
    ui->textEdit->setText(readInternalText(":/Assets/about-text/gplv3.html"));
}
