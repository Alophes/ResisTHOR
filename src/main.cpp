#include <Arduino.h>
#include <LibRobus.h>

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

void setup() {
  BoardInit();
  Serial.begin(9600);
}

void loop() {
  delay(100);
  Serial.println(ENCODER_Read(1));
}
