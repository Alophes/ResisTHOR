#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

#define TEST 1
#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 1


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
	
	// écrire quelle pin fait koi et est connecter a quoi
}


void loop(){

	Serial.println(ENCODER_Read(RIGHT));
	delay(1000);

	// Paramètrede base
	//test();
}

void test() {
	if(TEST != 1){
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
			readCommand(state.movement, state.nbmovement);

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

	if(TEST == 1){
		
		if(TEST_LECTUREPARCOUR){
			readCommand(state.movement, state.nbmovement);
		}

		if(TEST_FAIREPARCOUR){
			/*readCommand(state.movement);
			moving(state.movement, state.scAnswer, allStruct);*/

			
			
		}
	}

}