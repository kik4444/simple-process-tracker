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

#ifndef PARSER_H
#define PARSER_H

#include <QtCore>

class Parser
{
public:
    static QString parseDurationToString(quint64 duration)
    {
        quint64 minutes = duration / 60;
        quint64 hours = minutes / 60;
//        quint64 days = hours / 24;
        return QStringLiteral("%1").arg(hours, 2, 10, QLatin1Char('0')) + ":"
            + QStringLiteral("%1").arg(minutes % 60, 2, 10, QLatin1Char('0')) + ":"
            + QStringLiteral("%1").arg(duration % 60, 2, 10, QLatin1Char('0'));

    }

    static quint64 parseStringToDuration(QString durationInput)
    {
        QStringList splitDuration = durationInput.split(":");
        return splitDuration[0].toULongLong() * 3600 + splitDuration[1].toULongLong() * 60 + splitDuration[2].toULongLong();
    }
};

#endif // PARSER_H
