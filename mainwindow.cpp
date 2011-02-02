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
        r#: Red Val, Color #[1-4]
        g#: Green Val, Color #[1-4]
        b#: Blue Val, Color #[1-4]
 *******************************************************************/

#include <cstdlib>
#include <QtGui>
#include <QtOpenGL/QGLWidget>
#include <QXmlStreamReader>
#include <QTimer>
#include <GL/glu.h>
#include <qgl.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "flickersetting.h"

#define MAX_SPEED_VAL -1


/**
Constructor:
  Creates a RedGreenStrip and connects it to the UI
*/
MainWindow::MainWindow(int timerInterval)
{
    int width = 400; int height = 400;
    isSetMaxSpeed = false;

    ui.setupUi(this);
    setWindowTitle("Options");
    setFixedSize(this->size());

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
    connect( ui.hzSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateTimer()));
    connect( ui.boxSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateBoxes()));
    connect( ui.maxSpeed, SIGNAL(released()), this, SLOT(updateMaxSpeed()));
    connect( ui.saveSettings, SIGNAL(released()), this, SLOT(savePreset()));
    connect( ui.refreshSettings, SIGNAL(released()), this, SLOT(refreshPreset()));
    connect( ui.presetList, SIGNAL(pressed(QModelIndex)), this, SLOT(changePreset(QModelIndex)));

    QSlider* colors[12] = {ui.G1R1, ui.G1G1, ui.G1B1,ui.G1R2, ui.G1G2, ui.G1B2,
                           ui.G2R1, ui.G2G1, ui.G2B1,ui.G2R2, ui.G2G2, ui.G2B2};
    QLineEdit* texts[12] = {ui.G1R1T, ui.G1G1T, ui.G1B1T,
                            ui.G1R2T, ui.G1G2T, ui.G1B2T,
                            ui.G2R1T, ui.G2G1T, ui.G2B1T,
                            ui.G2R2T, ui.G2G2T, ui.G2B2T};

    for(int i=0; i<12; i++) {
        colorList[i] = colors[i];
        colorTextList[i] = texts[i];
        connect( colors[i], SIGNAL(valueChanged(int)), this, SLOT(updateColors()) );
    }

    // Set default vals
    numBoxes = 1;
    presetList = new QList<FlickerSetting>();
    presetText = new QStringList();
    loadPresets();
    if(!presetList->isEmpty())
        loadPreset(presetList->at(0));
}

/**
Update All:
    Calls all update functions
*/
void MainWindow::updateAll()
{
    updateColors(); // calls gradients
    updateTimer();
    updateMaxSpeed(false);
    updateBoxes();
}

/**
Update Boxes:
    Changes number of boxes displayed
*/
void MainWindow::updateBoxes()
{
    numBoxes = ui.boxSlider->sliderPosition();
    r->setBoxNum(numBoxes);

    ui.numBoxes->setText(QString::number(numBoxes));
}

/**
Update___
    Updates display with various information from sliders
*/
void MainWindow::updateTimer()
{
    int fps = ui.hzSlider->sliderPosition();
    r->setTimer(fps); // In frames per second

    ui.Hztext->setText(QString::number(fps) + "fps");
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
}
void MainWindow::updateMaxSpeed(bool hasChanged)
{
    if(hasChanged) isSetMaxSpeed = !isSetMaxSpeed;

    if(isSetMaxSpeed) {
        r->setTimer(MAX_SPEED_VAL);
        ui.hzSlider->setEnabled(false);
        ui.Hztext->setText("Max");
        ui.maxSpeed->setText("Custom Speed");
    } else {
        ui.hzSlider->setEnabled(true);
        updateTimer();
        ui.maxSpeed->setText("Max Speed");
    }
}


/**
Load Presets: Load list of presets
*/
void MainWindow::loadPresets()
{
    char* fileName;
    QFile* fp;
    QXmlStreamReader xmlr;

    // Get all .xml files
    QStringList fileTypes;
    fileTypes << "*.xml";
    QDir dir;
    QFileInfoList fileList = dir.entryInfoList(fileTypes);
    dir.setPath("presets");
    fileList.append(dir.entryInfoList(fileTypes));

    for(int i=0; i<fileList.size(); ++i) {
        QFileInfo fileInfo = fileList.at(i);
        fileName = fileInfo.absoluteFilePath().toAscii().data();
        fp = new QFile(fileName);
        if(!fp->exists()) break;

        if(!fp->open(QIODevice::ReadOnly)) {
            qDebug("Unexpected error opening preset!");
            return;
        }

        QString name = QString(fileInfo.baseName());

        bool isPresetFile = false; // Don't read random xml's
        xmlr.setDevice(fp);
        xmlr.readNext();

        // Initialize with default vals
        int colors[12]; int cindex = 0;
        int speed = 60;
        bool isMaxSpeed = false;
        int numBoxes = 1;

        while(!xmlr.atEnd()) {
            if(xmlr.isStartElement()) {
               QString name = xmlr.name().toString();
               xmlr.readNext();
               int text = atoi(xmlr.text().toString().toAscii());

               if(name == "FlickerOptions") {
                   isPresetFile = true;
                   continue;
               } else if(!isPresetFile) {
                   break; // Leave this file, not what we want
               } else if(name.at(0) == 'c') {
                   colors[cindex] = text;
                   ++cindex;
               } else if(name == "Speed") {
                   speed = text;
               } else if(name == "NumBoxes") {
                   numBoxes = text;
               } else if(name == "IsMaxSpeed") {
                   isMaxSpeed = text == 1 ? true : false;
               }
            }
            xmlr.readNext();
        }
        addPresets(name.toAscii(), colors, speed, isMaxSpeed, numBoxes);
        fp->close();
    }
}


/**
Clear Presets: Clear presets for reloading
*/
void MainWindow::clearPresets()
{
    presetList->clear();
    presetText->clear();
}


/**
Reset Presets: Reload from disk
*/
void MainWindow::refreshPreset()
{
    clearPresets();
    loadPresets();
}


/**
Add Presets: Add a single preset to the list
*/
void MainWindow::addPresets(const char* name,
                             int* color_preset,
                             int speed, bool isMaxSpeed,
                             int numBoxes)
{
    FlickerSetting preset(name, color_preset, speed, isMaxSpeed, numBoxes);

    presetList->append(preset);
    *presetText << name;

    ui.presetList->setModel(new QStringListModel(*presetText)); // Append to preset list
}



/**
Save Preset: Commit current settings to memory under filename preset#.xml
*/
void MainWindow::savePreset()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Preset",
                                "./presets", tr("XML Document (*.xml)"));

    if(fileName.isEmpty()) return;
    if(!fileName.endsWith(".xml")) fileName.append(".xml");
    QFile* fp = new QFile(fileName);

    if(!fp->open(QIODevice::WriteOnly)) {
        qDebug("Unexpected error saving preset");
        return;
    }
    QXmlStreamWriter xmlWriter(fp);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("FlickerOptions");

    // Write 12 colors
    char colorValText[4]; // Length 4: c12\0
    for(int i=0; i<12; i++) {
        sprintf(colorValText, "c%d", i);
        xmlWriter.writeTextElement(colorValText, QString::number(colorList[i]->value()));
    }

    xmlWriter.writeTextElement("Speed", QString::number(ui.hzSlider->value()));
    xmlWriter.writeTextElement("NumBoxes", QString::number(ui.boxSlider->value()));
    xmlWriter.writeTextElement("IsMaxSpeed", isSetMaxSpeed ? "1" : "0");

    xmlWriter.writeEndDocument();
    fp->close();

    // Reload
    refreshPreset();
}


/**
Change Presets: Switch to a different preset
*/
void MainWindow::changePreset(QModelIndex modelIndex)
{
    int row = modelIndex.row();
    loadPreset(presetList->at(row));
}

/**
Load preset: Loads the settings provided
  */
void MainWindow::loadPreset(FlickerSetting settings)
{
    // Hz
    ui.hzSlider->setSliderPosition(settings.speed);

    // Colors
    r->setColors(settings.colorVals);

    for(int i=0; i<12; i++) {
        colorList[i]->setValue(settings.colorVals[i]);
        colorTextList[i]->setText(QString::number(settings.colorVals[i]));
    }

    isSetMaxSpeed = settings.isMaxSpeed;
    numBoxes = settings.numBoxes;

    updateAll();
}

/**
Begin:
  Shows the display
*/
void MainWindow::beginSlot()
{
    // ui.beginButton->hide();

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
        m_timer = new QTimer( this );
        connect( m_timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()) );
        m_timer->start( timerInterval );
    }

    // Where in flicker?
    showingG1 = false;

    setBoxNum(1);
}

/**
Initialize painter:
  Sets some variables
*/
void Flickerer::initPainter()
{
    w = width(); h = height();
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
    if(hz==MAX_SPEED_VAL) val = 0;
    else if(hz <= 0) val = 100000;
    else val = 1000.0 / hz;
    m_timer->setInterval(val);
}

/**
Set Box Nums:
  Updates the number of boxes displayed
*/
void Flickerer::setBoxNum(int num)
{
    if(num < 1) num = 1;
    numBoxes = num;

    wLength = ceil((double)w / numBoxes);
    hLength = ceil((double)h / numBoxes);
    steps = 1.0f / (numBoxes > 1 ? (numBoxes-1) : 1); // For var amtC#inC#
}

/**
Set various colors:
  Updates the color in each gradient
*/
void Flickerer::setColors(int colorVals[]) {
    for(int i=0; i<12; i+=3) {
        float* curr_rgb;
        if(i==0)       curr_rgb = g1c1_rgb;
        else if(i==3)  curr_rgb = g1c2_rgb;
        else if(i==6)  curr_rgb = g2c1_rgb;
        else           curr_rgb = g2c2_rgb;

        curr_rgb[0] = colorVals[i+0] / 255.0;  // R
        curr_rgb[1] = colorVals[i+1] / 255.0;  // G
        curr_rgb[2] = colorVals[i+2] / 255.0;  // B
    }
}

/**
drawBackground
  The openGL scene to be placed in the viewport
*/
void Flickerer::drawBackground(QPainter *painter,
                                   const QRectF &)
{
//    qDebug("Drawing background");

//    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
//        && painter->paintEngine()->type() != QPaintEngine::OpenGL2)
//        qWarning("OpenGLScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");

    bool startedWithG1 = showingG1;

    painter->beginNativePainting();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    for(int col=0; col < numBoxes; ++col) {
        if(col % 2 == 0)
            showingG1 = startedWithG1;
        else
            showingG1 = !startedWithG1;

        float amtC2inC1 = 0.0f; // Also amtC1inC2
        float amtC1inC1 = 1.0f; // Also amtC2inC2

        for(int row=0; row < numBoxes; ++row) {
            // Where to begin drawing
            int wStart = col*w / numBoxes;
            int hStart = row*h / numBoxes;

            glBegin(GL_QUADS);

            float* c1 = showingG1 ? g1c1_rgb : g2c2_rgb;
            float* c2 = showingG1 ? g1c2_rgb : g2c1_rgb;
            float currC1[3]; float currC2[3];
            for(int i=0; i<3; i++) {
                currC1[i] = c1[i]*amtC1inC1 + c2[i]*amtC2inC1;
                currC2[i] = c2[i]*amtC1inC1 + c1[i]*amtC2inC1;
            }

            glColor3f(currC1[0], currC1[1], currC1[2]);
            glVertex2d(wStart, hStart);
            glVertex2d(wStart + wLength, hStart);

            if(numBoxes == 1)
                glColor3f(currC2[0], currC2[1], currC2[2]);
            glVertex2d(wStart + wLength, hStart + hLength);
            glVertex2d(wStart, hStart + hLength);

            glEnd();


            // How true to the c1/c2 gradient this row is
            amtC2inC1 += steps;      // Also amtC1inC2
            amtC1inC1 -= steps; // Also amtC2inC2

            showingG1 = !showingG1;
        }
    }
    glPopMatrix();
    painter->endNativePainting();

    showingG1 = !startedWithG1;
}
