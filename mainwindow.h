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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include <QGraphicsScene>
#include <QMessageBox>

#include "ui_mainwindow.h"

class Flickerer : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Flickerer(int timerInterval);
    void paintGL();
    void drawBackground(QPainter*, const QRectF&);
    void setTimer(int);

    void setColors(int vals[12]);
    void gradOffset(int, int);
    void setBrightScale(int vals[4]);
    void initPainter();

private:
    //used to refresh the scene at a given interval
    QTimer *m_timer;

    //displays
    QRect *rect;
    QPainter* p;
    QLinearGradient *g1, *g2, *curr;
    QColor *g1c1, *g1c2; float g1c1_rgb[3];
                         float g1c2_rgb[3];
    QColor *g2c1, *g2c2; float g2c1_rgb[3];
                         float g2c2_rgb[3];
    QPixmap *buffer1, *buffer2;
    int g1_offset, g2_offset;   // Gradient offset, -100-100
    int brightVals[4];          // Brightness scaling, 1-100%

    int w, h;
    int startx, starty, endx, endy;

    //flicker
    bool showingG1;

public slots:
  //slot used to refresh scene when invoked by m_timer
  void timeOutSlot();

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int timerInterval);
    Ui::MainWindow ui;

private:
    Flickerer *r;
    QGraphicsView *view;
    QGraphicsScene* scene;
    void sliderMoved(int);
    void presetWithColors(int colors[12]);
    QSlider* colorList[12];
    QLineEdit* colorTextList[12];
    QSlider* brightList[4];
    QLineEdit* brightTextList[4];

    QMessageBox* errmsg;

public slots:
    void updateAll();
    void updateColors();
    void updateTimer();
    void updateGradients();
    void updateBrightness();
    void preset1();
    void preset2();
    void beginSlot();
    void closeEvent(QCloseEvent *);
    void showBeginButton(); // Redisplay the begin button
};

#endif // MAINWINDOW_H
