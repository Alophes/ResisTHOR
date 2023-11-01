#include "lineDetector.h"

void LineDetectorInit() {
    pinMode(lineDetector.pin, INPUT);

}

LineDetector LineDetector_Read() {
    int data = analogRead(lineDetector.pin);
    Serial.println(data);
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