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
byte svastika[8] = {
  B00000,
  B00000,
  B11101,
  B00101,
  B11111,
  B10100,
  B10111,
  B00000
};

void setup() {
	lcdInit();
	Serial.begin(9600);
	delay(1000);
	
	char msg[] = "Tabaranak de colisse de viarge";
	// Your custom character data for a smiley face
	/*createCustomChar(0,smiley);
	lcdSetPos(0, 0);
	lcdPutc(0);
	delay(2000);
	lcdClear();

    lcdSetPos(0, 1);
    lcdPuts("bonjour alex");
	delay(2000);
	lcdClear();*/
	lcdLongTexte(0,0,0,msg);
	delay(1000);
	lcdLongTexte(0,0,0,msg);

}

void loop() {
	delay(500);
}