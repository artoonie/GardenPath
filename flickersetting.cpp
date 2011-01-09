#include "flickersetting.h"
#include "qdebug.h"

// (Dangerously) assumes correct number of values in passed in arrays
FlickerSetting::FlickerSetting(const char* myName,
                               int myColorVals[], int myBrightnessVals[],
                               int mySpeed, bool myIsMaxSpeed)
{
    name = myName;

    for(int i=0; i<12; i++)
        colorVals[i] = myColorVals[i];

    for(int i=0; i<4; i++)
        brightnessVals[i] = myBrightnessVals[i];

    speed = mySpeed;
    isMaxSpeed = myIsMaxSpeed;
}