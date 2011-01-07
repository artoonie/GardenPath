//This file is part of The Garden Path

//The Garden Path is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//The Garden Path is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with The Garden Path.  If not, see <http://www.gnu.org/licenses/>.

#include <QtPlugin>
#include <QtGui/QApplication>
//include <QApplication.h>
#include <QtOpenGL/QGLWidget>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow((int)(12.0/60.0 * 10));
    w->show();

    return a.exec();
}
