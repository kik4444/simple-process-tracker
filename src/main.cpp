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

#include "core/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName(QLatin1String("Simple Process Tracker"));
    QCoreApplication::setOrganizationName(QLatin1String("Simple Process Tracker"));

//    a.setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();
    return a.exec();
}
