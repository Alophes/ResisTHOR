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
	switch (rfidRead())
	{
	case rfid_command::forward :
		Serial.println("FORWARD");
		break;
	case rfid_command::left :
		Serial.println("LEFT");
		break;
	case rfid_command::rigth :
		Serial.println("RIGHT");
		break;
	case rfid_command::start :
		Serial.println("START");
		break;
	case rfid_command::stop :
		Serial.println("STOP");
		break;
	default:
		break;
	}
}