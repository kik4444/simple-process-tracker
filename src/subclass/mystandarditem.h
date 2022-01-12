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

#ifndef MYSTANDARDITEM_H
#define MYSTANDARDITEM_H

#include <QStandardItem>
#include "../core/parser.h"

class MyStandardItem : public QStandardItem
{

public:
    MyStandardItem() : QStandardItem() {};
    explicit MyStandardItem(const QString &text) : QStandardItem(text) {};
    MyStandardItem(const QIcon &icon, const QString &text) : QStandardItem(icon, text) {};
    explicit MyStandardItem(int rows, int columns = 1) : QStandardItem(rows, columns) {};
    virtual ~MyStandardItem() {};

    bool operator < (const QStandardItem &other) const
    {
        bool textConversionSuccess, otherConversionSuccess;
        Q_UNUSED(text().toULongLong(&textConversionSuccess));
        Q_UNUSED(other.text().toULongLong(&otherConversionSuccess));

        if (textConversionSuccess && otherConversionSuccess)
        {
            return text().toULongLong() < other.text().toULongLong();
        }
        else if (text().contains(":") && other.text().contains(":"))
        {
            if (Parser::parseStringToDuration(text()) && Parser::parseStringToDuration(other.text()))
                return Parser::parseStringToDuration(text()) < Parser::parseStringToDuration(other.text());
        }

        return text() < other.text();
    }
};

#endif // MYSTANDARDITEM_H
