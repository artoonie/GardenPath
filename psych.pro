#-------------------------------------------------
#
# Project created by QtCreator 2010-11-21T01:20:37
#
# This file is part of The Garden Path
#
# The Garden Path is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The Garden Path is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more dtails.
#
# You should have received a copy of the GNU General Public License
# along with The Garden Path.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------

QT += opengl

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

OTHER_FILES += COPYING

static {
    CONFIG += static
    QT += opengl
    DEFINES += STATIC
    message("Static build.")
}
