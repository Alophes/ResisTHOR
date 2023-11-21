#include <stdio.h>
#include "util.h"
#include "math.h"
#include <Arduino.h>
#include "Adafruit_TCS34725.h"
#include "lcd.h"
 uint8_t smiley[8] = {
  B01010,
  B01010,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
  };

void setup() {
	lcdInit();
	delay(1000);
	
  // Your custom character data for a smiley face
  createCustomChar(0,smiley);

  // Display the custom character at position (0, 0)
  lcdSetPos(0, 0);
  lcdPutc(0);

//	lcdTexte(0,0,"salut");

}

void loop() {
	delay(100);
}