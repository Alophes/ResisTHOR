#ifndef LINE_DETECTOR_H
#define LINE_DETECTOR_H

#include <Arduino.h>

struct LineDetector
{
    int pin = PINA3;
    bool right = false;
    bool left = false;
    bool middle = false;
};

void LineDetectorInit();
LineDetector LineDetector_Read();
bool LineDetector_Right();
bool LineDetector_Left();
bool LineDetector_Middle();

#endif