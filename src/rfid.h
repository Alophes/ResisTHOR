#ifndef RFID_H
#define RFID_H

#include <Arduino.h>
#include <string.h>

enum rfid_command {
    error = -1,
    forward,
    left,
    rigth,
    start,
    stop
};

void rfidInit();
rfid_command rfidCommand(String tag);
void rfidPrintCommand(rfid_command command);
rfid_command rfidRead();

#endif