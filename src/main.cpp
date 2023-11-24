#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"

#define TEST 1
#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 1
#define TESTMOVEMENT 0
#define TESTLECTEURLCD 0
#define TESTRFID 0

int MOTORCALIBRATION = 0;

BasicSettings baseSet;
Pin pin;
AllStruct *allStruct = initAllStruct(baseSet, pin);
State *state = allStruct->state;

void test();

void setup()
{

	//RFID
	rfidInit();

	//SDcard
	pinMode(11, OUTPUT);
	digitalWrite(11, HIGH);
	pinMode(53, OUTPUT);
	digitalWrite(53, HIGH);
	BoardInit();


	// detecteur de proximité
	pinMode(pin.capGauche, INPUT);		// Pin.capDroite
	pinMode(pin.capGauche, INPUT);		// capGauche
	pinMode(pin.led_capDroite, OUTPUT); // ledPin.capDroite
	pinMode(pin.led_capGauche, OUTPUT); // ledcapGauche

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

		if (TEST_LECTUREPARCOUR)
		{
			readCommand(state->movement);
		}

		if (TEST_FAIREPARCOUR)
		{
			readCommand(state->movement);
			moving(state->movement, state->scAnswer, allStruct);
			delay(1000);
			returnToBase(state->movement, state->scAnswer, allStruct);
		}

		if (TESTMOVEMENT)
		{
			testMovement(allStruct);
		}

		if(TESTLECTEURLCD){

			choseParkour(allStruct);
			while(1){

				Serial.println(ENCODER_Read(0));
				delay(500);

				if(ROBUS_IsBumper(LEFT)){
					while(ROBUS_IsBumper(LEFT)){delay(50);};
					delay(50);
					break;
				}
			}
		}

		if(TESTRFID){
			rfidPrintCommand(rfidRead());
		}
	}

	if (MOTORCALIBRATION == 1)
	{

		motorCalibration(allStruct);
		MOTORCALIBRATION = 0;
	}



}