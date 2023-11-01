#include "lineDetector.h"

LineDetector lineDetector;

void LineDetectorInit() {
    pinMode(lineDetector.pinLeft, INPUT);
    pinMode(lineDetector.pinRight, INPUT);
    pinMode(lineDetector.pinMiddle, INPUT);
}

LineDetector LineDetector_Read() {
    lineDetector.left = digitalRead(lineDetector.pinLeft);
    lineDetector.right = digitalRead(lineDetector.pinRight);
    lineDetector.middle = digitalRead(lineDetector.pinMiddle);
    return lineDetector;
}

bool LineDetector_Right() {
    LineDetector_Read();
    return lineDetector.right;
}

bool LineDetector_Left() {
    LineDetector_Read();
    return lineDetector.left;
}

bool LineDetector_Middle() {
    LineDetector_Read();
    return lineDetector.middle;
}