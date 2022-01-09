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

/* Credit: https://stackoverflow.com/questions/63177587/pyqt-tableview-align-icons-to-center
 * and: https://stackoverflow.com/questions/2597534/is-there-a-way-to-display-icons-in-qlistview-without-text
 */

#ifndef MYITEMDELEGATE_H
#define MYITEMDELEGATE_H

#include <QStyledItemDelegate>

class MyItemDelegate : public QStyledItemDelegate
{
    void initStyleOption(QStyleOptionViewItem* item, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(item, index);
        item->decorationSize = item->rect.size();
        item->displayAlignment = Qt::AlignCenter;
    }
};

#endif // MYITEMDELEGATE_H
