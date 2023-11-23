#include <Arduino.h>
#include <LibRobus.h>
#include <Grove_I2C_Color_Sensor_TCS3472-master/Adafruit_TCS34725.h>
#include "util.h"
#include "math.h"
#include "rfid.h"

void setup() {
	BoardInit();
	rfidInit();
}

void loop() {
	rfidPrintCommand(rfidRead());
}