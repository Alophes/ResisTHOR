#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"
#include "lcd.h"

#define TEST 0

int MOTORCALIBRATION = 0;

BasicSettings baseSet;
Pin pin;
AllStruct *allStruct = initAllStruct(baseSet, pin);
State *state = allStruct->state;

void test();

void setup()
{

	// LCD
	lcdInit();
	delay(250);

	// RFID
	rfidInit();

	// SDcard
	pinMode(11, OUTPUT);
	digitalWrite(11, HIGH);
	pinMode(53, OUTPUT);
	digitalWrite(53, HIGH);
	BoardInit();

	// detecteur de proximité
	pinMode(pin.capGauche, INPUT); // Pin.capDroite
	pinMode(pin.capGauche, INPUT); // capGauche

	// potentionmètre ajustement mouvement
	pinMode(pin.potentiometerForward, INPUT);
	pinMode(pin.potentiometerTurnRight, INPUT);

	// écrire quelle pin fait koi et est connecter a quoi
}

void loop()
{
	if (MOTORCALIBRATION == 1)
	{

		motorCalibration(allStruct);
		MOTORCALIBRATION = 0;
	}

	if (TEST == 0)
	{
		if (state->start == 1)
		{
			printLCD(MENU, allStruct);

			while (readRIFD() != START)
			{
				delay(100);
			}

			choseParkour(allStruct);
			state->start = 0;
		}

		if (state->start == 0)
		{
			// Lecture des commandes
			readCommand(allStruct);

			// il va faire les mouvement jusqu'au sccan
			printLCD(MOVING, allStruct);
			printLCD(HAPPYFACE, allStruct);
			state->nbOfMovement = moving(state->movement, state->scAnswer, allStruct);
			delay(2000);
			returnToBase(state->movement, allStruct);

			if (verifieAnswer(state->realAnswer, state->scAnswer) == 0)
			{
				printLCD(SADFACE, allStruct);
				delay(2000);
			}
			
			else
			{
				printLCD(HAPPYFACE, allStruct);
				state->realAnswer = 1;
			}
		}
	}

	if (TEST == 1)
	{
		test(allStruct);
	}
}