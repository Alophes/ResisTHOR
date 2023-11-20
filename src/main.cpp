#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"

State etat;

void setup(){
	BoardInit();
	etat.bonneRéponse = 1;
	
	// écrire quelle pin fait koi et est connecter a quoi
}


void loop(){

	// Paramètrede base

	if(etat.bonneRéponse == 1)
	{
		etat.bonneRéponse = 0;
		etat.question = choseParkour();


		//*etat.reponse
		etat.nbAnswer = sizeof(etat.reponse);
	}
	
	if(etat.bonneRéponse == 0)
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
			etat.bonneRéponse = 1;
		}
	}
	
}