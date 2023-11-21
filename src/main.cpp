#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"


#define TEST 1
#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 1


AllStruct allStruct;
State state = allStruct.state;
BasicSettings baseSet;
	Pin pin;
void setup(){
	BoardInit();
	allStruct.state = initState();
	allStruct.speed = initSpeed();
	allStruct.initialSpeed = initSpeed();
	allStruct.pulse = initPulse();
	
	// écrire quelle pin fait koi et est connecter a quoi
}


void loop(){

	// Paramètrede base

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

	if(TEST == 1){
		
		if(TEST_LECTUREPARCOUR){
			readCommand(state.movement);
		}

		if(TEST_FAIREPARCOUR){
			readCommand(state.movement);
			
		}
	}
	
}