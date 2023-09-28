#include <Arduino.h>
#include <LibRobus.h>

#define RIGHT_PROX 44
#define LEFT_PROX 42

void setup() {

    BoardInit();

    pinMode(RIGHT_PROX, INPUT);
    pinMode(LEFT_PROX, INPUT);

}

int getRightProx() { return digitalRead(RIGHT_PROX); }
int getLeftProx() { return digitalRead(LEFT_PROX); }


void loop() {

    if (getLeftProx()) { Serial.println("Left proximity detected"); }
    if (getRightProx()) { Serial.println("Right proximity detected"); }

    delay(100);

}