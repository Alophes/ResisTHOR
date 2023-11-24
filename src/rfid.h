#ifndef RFID_H
#define RFID_H
#define FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define SCAN 4
#define STOP 5

#include <Arduino.h>
#include <string.h>

enum rfid_command {
    error = -1,
    fOrward,
    left,
    rigth,
    scan,
    stop
};



void rfidInit();
rfid_command rfidCommand(String tag);
void rfidPrintCommand(rfid_command command);
rfid_command rfidRead();
int rfidReturnCommand(rfid_command command);

#endif