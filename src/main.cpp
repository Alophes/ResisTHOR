#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"

#define TEST 1
#define TEST_LECTUREPARCOUR 1

State etat;

void setup(){
	BoardInit();
	
	// écrire quelle pin fait koi et est connecter a quoi
}


void loop(){

	// Paramètrede base

	if(TEST != 1){
		if(etat.bonneReponse == 1)
		{
			etat.bonneReponse = 0;
			etat.question = choseParkour();


			//*etat.reponse
			etat.nbAnswer = sizeof(etat.reponse);
		}
		
		if(etat.bonneReponse == 0)
		{
			//Lecture des commandes
			readCommand(etat.movement);

			//il va faire les mouvement jusqu'au sccan
			moving(etat.movement, etat.scAnswer);



			if (verifieAnswer(etat.reponse, etat.nbAnswer, etat.scAnswer) == 0){
				//danse défaite
			}
			else{
				//danse victoire
				etat.bonneReponse = 1;
			}
		}
	}

	if(TEST == 1){
		
		if(TEST_LECTUREPARCOUR){
			readCommand(etat.movement);
		}
	}
	
}