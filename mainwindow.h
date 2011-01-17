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
#include "flickersetting.h"

class Flickerer : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Flickerer(int timerInterval);
    void paintGL();
    void drawBackground(QPainter*, const QRectF&);
    void setTimer(int);
    void setBoxNum(int);

    void setColors(int vals[12]);
    void initPainter();

private:
    //used to refresh the scene at a given interval
    QTimer *m_timer;

    //displays
    float g1c1_rgb[3];
    float g1c2_rgb[3];
    float g2c1_rgb[3];
    float g2c2_rgb[3];

    int w, h;

    //boxes
    int numBoxes;
    int wLength, hLength; // How big each box is
    float steps; // For updating var amtC#inC#

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
    // Display
    Flickerer *r;
    QGraphicsView *view;
    QGraphicsScene* scene;
    QSlider* colorList[12];
    QLineEdit* colorTextList[12];
    QMessageBox* errmsg;

    bool isSetMaxSpeed;

    QList<FlickerSetting>* presetList;
    QStringList* presetText;
    // Load all presets from disk
    void loadPresets();
    // Add a single preset to preset list
    void addPresets(const char* name,
                    int* color_preset,
                    int speed, bool isMaxSpeed);
    // Set up a single preset to display
    void loadPreset(FlickerSetting);
    // Clear presets for reloading
    void clearPresets();

public slots:
    void updateAll();
    void updateColors();
    void updateTimer();
    void updateBoxes();
    void updateMaxSpeed(bool hasChanged = true); // If hasChanged, flip bool
    void changePreset(QModelIndex);
    void beginSlot();
    void closeEvent(QCloseEvent *);
    void showBeginButton(); // Redisplay the begin button
    void savePreset();
    void refreshPreset();
};

#endif // MAINWINDOW_H
