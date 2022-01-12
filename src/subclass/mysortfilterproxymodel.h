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

#ifndef MYSORTFILTERPROXYMODEL_H
#define MYSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "mystandarditemmodel.h"
#include "../core/parser.h"

class MySortFilterProxyModel : public QSortFilterProxyModel
{

public:
    explicit MySortFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {};
    ~MySortFilterProxyModel() {};

    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
    {
        switch (source_left.column())
        {
            case ProcessColumns::Number:
                return source_left.data().toUInt() < source_right.data().toUInt();

            case ProcessColumns::Duration:
                return Parser::parseStringToDuration(source_left.data().toString()) < Parser::parseStringToDuration(source_right.data().toString());

            default:
                return QSortFilterProxyModel::lessThan(source_left, source_right);
        }
    }
};

#endif // MYSORTFILTERPROXYMODEL_H
