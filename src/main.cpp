#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"
#include "lcd.h"

#define TEST 1


int MOTORCALIBRATION = 0;

BasicSettings baseSet;
Pin pin;
AllStruct *allStruct = initAllStruct(baseSet, pin);
State *state = allStruct->state;

void test();

void setup()
{

	//LCD
	lcdInit();
	delay(250);

	//RFID
	rfidInit();

	//capteur de couleur


	//SDcard
	pinMode(11, OUTPUT);
	digitalWrite(11, HIGH);
	pinMode(53, OUTPUT);
	digitalWrite(53, HIGH);
	BoardInit();


	// detecteur de proximité
	pinMode(pin.capGauche, INPUT);		// Pin.capDroite
	pinMode(pin.capGauche, INPUT);		// capGauche

	

	// lumière du scan
	pinMode(pin.ledScan, OUTPUT);
	digitalWrite(pin.ledScan, LOW);

	// potentionmètre ajustement mouvement
	pinMode(pin.potentiometerForward, INPUT);
	pinMode(pin.potentiometerTurn, INPUT);

	// écrire quelle pin fait koi et est connecter a quoi
}

void loop()
{
	if (TEST == 0)
	{
		if (state->start == 1)
		{
			printLCD(MENU);
			choseParkour(allStruct);
			state->start = 0;
		}

		if (state->start == 0)
		{
			// Lecture des commandes
			readCommand(state->movement);

			// il va faire les mouvement jusqu'au sccan
			moving(state->movement, state->scAnswer, allStruct);

			if (verifieAnswer(state->reponse, state->nbAnswer, state->scAnswer) == 0)
			{
				// danse défaite
			}
			else
			{
				// danse victoire
				state->realAnswer = 1;
			}
		}
	}

	if (TEST == 1)
	{
		test(allStruct);
		
	}

	if (MOTORCALIBRATION == 1)
	{

		motorCalibration(allStruct);
		MOTORCALIBRATION = 0;
	}



}