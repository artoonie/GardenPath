#ifndef FLICKERSETTING_H
#define FLICKERSETTING_H

class FlickerSetting
{
public:
    FlickerSetting(const char* name,
                   int myColorVals[], int myBrightnessVals[],
                   int mySpeed, bool myIsMaxSpeed);
    const char* name;
    int colorVals[12]; // 12 colors
    int brightnessVals[4]; // 4 brightness vals
    int speed; // Hz
    bool isMaxSpeed; // Max speed activated?
};

#endif // FLICKERSETTING_H
