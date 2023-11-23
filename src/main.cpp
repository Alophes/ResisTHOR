#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"


#define TEST 1
#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 0
#define TESTMOVEMENT 1

int MOTORCALIBRATION = 0;


AllStruct allStruct;
State state = allStruct.state;
BasicSettings baseSet;
Pin pin;

void test();

void setup(){
	BoardInit();
	allStruct.state = initState();
	allStruct.speed = initSpeed(baseSet);
	allStruct.initialSpeed = initSpeed(baseSet);
	allStruct.pulse = initPulse();

	//detecteur de proximité
	pinMode(pin.capGauche, INPUT); //Pin.capDroite
	pinMode(pin.capGauche, INPUT); //capGauche
	pinMode(pin.led_capDroite, OUTPUT); //ledPin.capDroite
	pinMode(pin.led_capGauche, OUTPUT); //ledcapGauche

	//lumière du scan
	pinMode(pin.ledScan, OUTPUT);
	digitalWrite(pin.ledScan, LOW);

	//potentionmètre ajustement mouvement
	pinMode(pin.potentiometerForward, INPUT);
	pinMode(pin.potentiometerTurn, INPUT);

	
	// écrire quelle pin fait koi et est connecter a quoi
}



void loop() {
	if(!TEST){
		if(state.bonneReponse == 1)
		{
			state.bonneReponse = 0;
			state.question = choseParkour();


			//*state.reponse
			state.nbAnswer = sizeof(state.reponse);
		}
		
		if(state.bonneReponse == 0)
		{
			//Lecture des commandes
			readCommand(state.movement);

			//il va faire les mouvement jusqu'au sccan
			moving(state.movement, state.scAnswer, allStruct);



			if (verifieAnswer(state.reponse, state.nbAnswer, state.scAnswer) == 0){
				//danse défaite
			}
			else{
				//danse victoire
				state.bonneReponse = 1;
			}
		}
	}

	if(TEST){
		
		if(TEST_LECTUREPARCOUR){
			readCommand(state.movement);
		}

		if(TEST_FAIREPARCOUR){
			readCommand(state.movement);
			moving(state.movement, state.scAnswer, allStruct);
			returnToBase(state.movement, state.scAnswer, allStruct);
			
			
			
		}

		if(TESTMOVEMENT){
			testMovement(allStruct);
		}
	}

	if(MOTORCALIBRATION){

		motorCalibration(allStruct);
		MOTORCALIBRATION = 0;
	}



}