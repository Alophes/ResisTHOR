
#include <Arduino.h>
#include <LibRobus.h>


float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }


void setup() {

    BoardInit();

    pinMode(PINA0, INPUT);
    pinMode(PINA1, INPUT);

}


void loop() {

    delay(100);

}