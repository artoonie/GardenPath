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


/*******************************************************************
    This program allows you two flicker between two gradients with
    various options available.

    The naming convention in variables in this file are as follows:
    Prefixes or standalone:
        g#: Gradient #[1, 2]
        c1: Gradient 1, Color 1
        c2: Gradient 1, Color 2
        c3: Gradient 2, Color 1
        c4: Gradient 2, Color 2
    Suffixes (always accompanied by one of the above):
        br#: Brightness of color #[1-4]
        r#: Red Val, Color #[1-4]
        g#: Green Val, Color #[1-4]
        b#: Blue Val, Color #[1-4]
 *******************************************************************/

#include <QtGui>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include <GL/glu.h>
#include <qgl.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


/**
Constructor:
  Creates a RedGreenStrip and connects it to the UI
*/
MainWindow::MainWindow(int timerInterval)
{
    int width = 400; int height = 400;

    ui.setupUi(this);
    setWindowTitle("Options");

    // Set up graphics viewer
    r = new Flickerer(timerInterval);
    // r->resize(width, height);
    r->setSceneRect(0, 0, width, height);
    r->initPainter();

   // scene = new QGraphicsScene(0, 0, width, height);
   // scene->addWidget(r);

    QGLFormat* fmt = new QGLFormat();
    fmt->setSwapInterval(1);
    QGLWidget* w = new QGLWidget(*fmt);
    // w->setFormat(*fmt);

    int success = w->format().swapInterval(); // Should be 1 if hardware supports
    if(success != 1) {
        qDebug("Hardware does not support vsync.");
        errmsg = new QMessageBox(QMessageBox::Warning, "Error",
                           "Your hardware cannot properly display this illusion.",
                           QMessageBox::Ignore | QMessageBox::Abort,
                           this);
        if(errmsg->exec() == QMessageBox::Abort) {
            exit(0);
        }
    }

    view = new QGraphicsView(r, NULL);
    view->setViewport(w);
    view->setViewportUpdateMode(
            QGraphicsView::FullViewportUpdate);
    view->resize(width, height);
    view->setWindowTitle("Finding the Garden Path");
    view->setMaximumSize(width, height);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // r->setFormat(fmt);

    connect( ui.beginButton, SIGNAL(released()), this, SLOT(beginSlot()) );
    connect( ui.g1Center, SIGNAL(sliderMoved(int)), this, SLOT(updateGradients()));
    connect( ui.g2Center, SIGNAL(sliderMoved(int)), this, SLOT(updateGradients()));
    connect( ui.hzSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateTimer()));
    connect( ui.Preset1, SIGNAL(released()), this, SLOT(preset1()));
    connect( ui.Preset2, SIGNAL(released()), this, SLOT(preset2()));

    QSlider* colors[12] = {ui.G1R1, ui.G1G1, ui.G1B1,ui.G1R2, ui.G1G2, ui.G1B2,
                           ui.G2R1, ui.G2G1, ui.G2B1,ui.G2R2, ui.G2G2, ui.G2B2};
    QLineEdit* texts[12] = {ui.G1R1T, ui.G1G1T, ui.G1B1T,
                            ui.G1R2T, ui.G1G2T, ui.G1B2T,
                            ui.G2R1T, ui.G2G1T, ui.G2B1T,
                            ui.G2R2T, ui.G2G2T, ui.G2B2T};
    QSlider* brightness[4] = {ui.C1Br, ui.C2Br, ui.C3Br, ui.C4Br};
    QLineEdit* brightTexts[4] = {ui.G1Br1T, ui.G1Br2T, ui.G2Br1T, ui.G2Br2T};

    for(int i=0; i<12; i++) {
        colorList[i] = colors[i];
        colorTextList[i] = texts[i];
        connect( colors[i], SIGNAL(sliderMoved(int)), this, SLOT(updateColors()) );
    }
    for(int i=0; i<4; i++) {
        brightList[i] = brightness[i];
        brightTextList[i] = brightTexts[i];
        connect( brightness[i], SIGNAL(sliderMoved(int)), this, SLOT(updateBrightness()) );
    }

    // Set default vals
    preset1();
}

/**
Update All:
    Calls all update functions
*/
void MainWindow::updateAll()
{
    updateBrightness();
    updateColors(); // calls gradients
    updateTimer();
}

/**
Update___
    Updates display with various information from sliders
*/
void MainWindow::updateBrightness()
{
    // Brightness: Set max in UI
    int brightVals[4];
    for(int i=0; i<4; i++) {
        brightVals[i] = brightList[i]->value();
        brightTextList[i]->setText(QString::number(brightVals[i]));
    }

    r->setBrightScale(brightVals);
    updateColors();
}
void MainWindow::updateGradients()
{
    // Center of gradients
    int gOff = ui.g1Center->value();
    int rOff = ui.g2Center->value();

    r->gradOffset(gOff, rOff);

    ui.g1Off->setText(QString::number(gOff));
    ui.g2Off->setText(QString::number(rOff));
}
void MainWindow::updateTimer()
{
    int hz = ui.hzSlider->sliderPosition();
    r->setTimer(hz); // In hertz

    ui.Hztext->setText(QString::number(hz) + "Hz");
}
void MainWindow::updateColors()
{
    // Get color values into array and update text
    int colorVals[12];
    for(int i=0; i<12; i++) {
        colorVals[i] = colorList[i]->value();
        colorTextList[i]->setText(QString::number(colorVals[i]));
    }

    // Update text color of each
    QLineEdit* texts[4] = {ui.g1c1t, ui.g1c2t, ui.g2c1t, ui.g2c2t};
    for(int i=3; i<=12; i+=3) {
        char rgbtext[56];
        sprintf(rgbtext,
                "background-color: rgb(%d,%d,%d);"
                "color: rgb(%d,%d,%d);",
                colorVals[i-3], colorVals[i-2], colorVals[i-1],
                255-colorVals[i-3], 255-colorVals[i-2], 255-colorVals[i-1]);
        texts[i/3 - 1]->setStyleSheet(rgbtext);
    }

    // Display
    r->setColors(colorVals);

    // Update the gradient to set the colors
    updateGradients();
}


/**
Preset 1: Red->Green, then Green->Red
*/
void MainWindow::preset1()
{
    // Colors
    int colors[12] =
     {127, 0, 0,
      0, 255, 0,
      0, 255, 0,
      127, 0, 0};

    presetWithColors(colors);
}

/**
Preset 2: Green->green, then Red->red
*/
void MainWindow::preset2()
{
    int colors[12] =
     {0, 255, 0,
      0, 70, 0,
      70, 0, 0,
      255, 0, 0};

    presetWithColors(colors);
}

/**
Preset with Colors:
  Returns all sliders and the display to the given preset
  */
void MainWindow::presetWithColors(int colors[12])
{
    // Hz
    ui.hzSlider->setSliderPosition(60);

    // Brightnesses
    for(int i=0; i<4; i++) {
        brightList[i]->setValue(100);
        brightTextList[i]->setText("100");
    }
    updateBrightness();

    // Colors
    r->setColors(colors);

    for(int i=0; i<12; i++) {
        colorList[i]->setValue(colors[i]);
        colorTextList[i]->setText(QString::number(colors[i]));
    }

    // Gradient offsets
    ui.g1Center->setValue(0);
    ui.g2Center->setValue(0);

    updateAll();
}

/**
Begin:
  Shows the display
*/
void MainWindow::beginSlot()
{
    // ui.beginButton->hide();

    //r->move(x()+width()+10,y());
    //r->show();
    view->move(x()+width()+10,y());
    view->show();

    update();
}

/**
ShowBeginButton:
  When display window closes, reshow the begin button
*/
void MainWindow::showBeginButton() {
    ui.beginButton->show();
}

/**
Close:
  Closes the display when the options window closes
*/
void MainWindow::closeEvent(QCloseEvent *) {
    view->close();
}

/**
Constructor:
  Creates a timer to update as often as user requested
*/
Flickerer::Flickerer(int timerInterval)
{
    if( timerInterval == 0 )
        m_timer = 0;
    else
    {
        // sets up a QTimer object to repaint the scene at the given rate (in milliseconds)
        m_timer = new QTimer( this );
        connect( m_timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()) );
        m_timer->start( timerInterval );
    }

    // Where in flicker?
    showingG1 = false;

    g1_offset=0; g2_offset=0;
}

/**
Initialize painter:
  Creates default colors and a new Painter object
*/
void Flickerer::initPainter()
{
    w = width(); h = height();
    startx = -w/2; endx = w/2;
    starty = -h/2; endy = h/2;

    rect = new QRect(0,0, w, h);

    g1c1 = new QColor(127,0,0);
    g1c2 = new QColor(0,255,0);
    g2c1 = new QColor(127,0,0);
    g2c2 = new QColor(0,255,0);

    gradOffset(0, 0);

    // Init painters
    p = new QPainter();

    // buffer1 = new QPixmap(rect->width(), rect->height());
    // buffer2 = new QPixmap(rect->width(), rect->height());
    // setAttribute(Qt::WA_OpaquePaintEvent);
}

/**
Timeout Slot:
  Show the other gradient and repaint
*/
void Flickerer::timeOutSlot()
{
    // showingG1 = !showingG1;
    // curr = showingG1 ? g1 : g2;
    update();
}

/**
Set timer:
  Updates the timer speed
*/
void Flickerer::setTimer(int hz)
{
    int val;
    if(hz <= 0) val = 100000;
    else if(hz>=60) val = 0;
    else val = 1000.0 / hz;
    m_timer->setInterval(val);
}

/**
Set various colors:
  Updates the color in each gradient
*/
void Flickerer::setColors(int colorVals[]) {
    QColor* colorDisp[4] = {g1c1, g1c2, g2c1, g2c2};

    for(int i=0; i<12; i+=3) {
        float* curr_rgb;
        if(i==0)       curr_rgb = g1c1_rgb;
        else if(i==3)  curr_rgb = g1c2_rgb;
        else if(i==6)  curr_rgb = g2c1_rgb;
        else           curr_rgb = g2c2_rgb;

        double scale = (brightVals[i/3]/100.0);

        curr_rgb[0] = colorVals[i+0]*scale / 255.0;  // R
        curr_rgb[1] = colorVals[i+1]*scale / 255.0;  // G
        curr_rgb[2] = colorVals[i+2]*scale / 255.0;  // B

        colorDisp[i/3]->setRgb(curr_rgb[0], curr_rgb[1], curr_rgb[2]);
    }

//    p->begin(buffer1);
//    p->fillRect(*rect, *g1);
//    p->end();

//    p->begin(buffer2);
//    p->fillRect(*rect, *g2);
//    p->end();
}

/**
Set Bright Scale:
    Sets the brightness scale factor, 1-100%
*/
void Flickerer::setBrightScale(int vals[4])
{
    for(int i=0; i<4; i++) {
        int val = vals[i];
        if(val<1) val=1;
        else if(val>100) val=100;
        brightVals[i] = val;
    }
}

/**
Gradient offset:
   Updates the offsets in each gradient
*/
void Flickerer::gradOffset(int g1_off, int g2_off) {
    g1_offset = g1_off;
    g2_offset = g2_off;


//    double g1_off1=0, g1_off2=0, g2_off1=0, g2_off2=0;

//    if(g1_off > 0)      g1_off1=g1_off/100.0;
//    else if(g1_off < 0) g1_off2=g1_off/100.0;

//    if(g2_off > 0)      g2_off1=g2_off/100.0;
//    else if(g2_off < 0) g2_off2=g2_off/100.0;

//    if(g1==NULL) { free(g1); free(g2); }

//    g1 = new QLinearGradient(0, h, w, h);
//    g2 = new QLinearGradient(0, h, w, h);
//    g1->setColorAt(g1_off1,     *g1c1);
//    g1->setColorAt(1.0+g1_off2, *g1c2);
//    g2->setColorAt(g2_off1,     *g2c1);
//    g2->setColorAt(1.0+g2_off2, *g2c2);
}


/**
drawBackground
  The openGL scene to be placed in the viewport
*/
void Flickerer::drawBackground(QPainter *painter,
                                   const QRectF &)
{
    // qDebug("Drawing background");

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
        && painter->paintEngine()->type() != QPaintEngine::OpenGL2)
        qWarning("OpenGLScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");

    painter->beginNativePainting();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float *c1_rgb, *c2_rgb;
    if(showingG1) {
        c1_rgb = g1c1_rgb;
        c2_rgb = g1c2_rgb;
    } else {
        c1_rgb = g2c1_rgb;
        c2_rgb = g2c2_rgb;
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_QUADS);

    glColor3f(c1_rgb[0], c1_rgb[1], c1_rgb[2]);
    glVertex2d(0, 0);
    glVertex2d(0, h);

    glColor3f(c2_rgb[0], c2_rgb[1], c2_rgb[2]);
    glVertex2d(w, h);
    glVertex2d(w, 0);

    glEnd();
    glPopMatrix();
    painter->endNativePainting();

    showingG1 = !showingG1;
}

/**
Repaint:
  Redraws the gradient (without OpenGL)
*/
/*
void Flickerer::paintEvent(QPaintEvent*)
{
//    qDebug("Painting.");
//    p->beginNativePainting();
//    glClearColor(.5f, .5f, 1.0f, 1.0f);
//    if(showingG1)
//        p->drawPixmap(*rect, *buffer1);
//    else
//        p->drawPixmap(*rect, *buffer2);
//    p->endNativePainting();
//    qDebug("Painting");

//    if (p->paintEngine()->type() != QPaintEngine::OpenGL
//        && p->paintEngine()->type() != QPaintEngine::OpenGL2)
//        qWarning("OpenGLScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");

    // p->beginNativePainting();


    float *c1_rgb, *c2_rgb;
    if(showingG1) {
        c1_rgb = g1c1_rgb;
        c2_rgb = g1c2_rgb;
    } else {
        c1_rgb = g2c1_rgb;
        c2_rgb = g2c2_rgb;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_QUADS);

    glColor3f(c1_rgb[0], c1_rgb[1], c1_rgb[2]);
    glVertex2d(startx, starty);
    glVertex2d(startx, endy);

    glColor3f(c2_rgb[0], c2_rgb[1], c2_rgb[2]);
    glVertex2d(endx, endy);
    glVertex2d(endx, starty);

    glEnd();
    glPopMatrix();
    // p->endNativePainting();

    showingG1 = !showingG1;
}
*/
