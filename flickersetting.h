#ifndef FLICKERSETTING_H
#define FLICKERSETTING_H

class FlickerSetting
{
public:
    FlickerSetting(const char* name,
                   int myColorVals[],
                   int mySpeed, bool myIsMaxSpeed,
                   int numBoxes);
    const char* name;
    int colorVals[12]; // 12 colors
    int speed; // Hz
    bool isMaxSpeed; // Max speed activated?
    int numBoxes; // Number of boxes across
};

#endif // FLICKERSETTING_H
