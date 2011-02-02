#include "flickersetting.h"
#include "qdebug.h"

// (Dangerously) assumes correct number of values in passed in arrays
FlickerSetting::FlickerSetting(const char* myName,
                               int myColorVals[],
                               int mySpeed, bool myIsMaxSpeed,
                               int myNumBoxes)
{
    name = myName;

    for(int i=0; i<12; i++)
        colorVals[i] = myColorVals[i];

    speed = mySpeed;
    isMaxSpeed = myIsMaxSpeed;
    numBoxes = myNumBoxes;
}
