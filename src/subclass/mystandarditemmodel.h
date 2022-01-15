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

// Credit: https://forum.qt.io/topic/20786/solved-how-can-i-make-a-particular-column-as-non-editable-in-qtableview/7

#ifndef MYSTANDARDITEMMODEL_H
#define MYSTANDARDITEMMODEL_H

#include <QStandardItemModel>

QT_BEGIN_NAMESPACE

namespace ProcessColumns
{
    enum ProcessColumns
    {
        HiddenCategories,
        Number,
        Tracking,
        Icon,
        Name,
        Duration,
        Notes,
        LastSeen,
        DateAdded
    };
}

namespace CategoryColumns
{
    enum CategoryColumns
    {
        HiddenNumber,
        Name
    };
}

QT_END_NAMESPACE

class MyStandardItemModel : public QStandardItemModel
{

private:
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Qt::ItemFlags itemFlags = QStandardItemModel::flags(index);
        return index.column() == ProcessColumns::Notes ? itemFlags : itemFlags & ~Qt::ItemIsEditable;
    }
};

#endif // MYSTANDARDITEMMODEL_H
