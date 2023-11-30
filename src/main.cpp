#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"
#include "lcd.h"

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

	BoardInit();

	// detecteur de proximité
	pinMode(pin.capGauche, INPUT); // Pin.capDroite
	pinMode(pin.capDroite, INPUT); // capGauche

	// potentionmètre ajustement mouvement
	pinMode(pin.potentiometerForward, INPUT);
	pinMode(pin.potentiometerTurnRight, INPUT);

	// écrire quelle pin fait koi et est connecter a quoi
}

void loop()
{
	if (baseSet.CALIBRATEMOTORS == 1)
	{

		motorCalibration(allStruct);
		baseSet.CALIBRATEMOTORS = 0;
	}

	if (TEST == 0)
	{
		if (state->start == 1)
		{
			printLCD(MENU, allStruct);

			while (1)
			{
				printLCD(MENU, allStruct);
				int choice = readRIFD();

				if(choice == START){
					break;
				}

				if(choice == STOP){
					printLCD(TESTMOVEMENT, allStruct);
					testMovement(allStruct);
					delay(200);
				}
				delay(100);
			}

			choseParkour(allStruct);
			state->start = 0;
		}

		if (state->start == 0)
		{
			// Lecture des commandes
			readCommand(allStruct);

			// il va faire les mouvement jusqu'au scan
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
				state->start = 1;
			}
		}
	}

	if (TEST == 1)
	{
		test(allStruct);
	}
}