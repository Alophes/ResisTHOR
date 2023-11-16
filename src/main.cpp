#include <stdio.h>
#include <Arduino.h>
#include <Adafruit_TCS34725.h>

//couleur
#define BLEU 1
#define VERT 2
#define JAUNE 3
#define ROUGE 4
#define BLANC 5

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup() {
	
	//BoardInit();
	
	if (tcs.begin()) {
    	Serial.println("Found sensor");
	} else {
    	Serial.println("No TCS34725 found ... check your connections");
    	while (1);
  }

}


int detectColor(){

	const int taille=100;			//Valeur à possiblement changer
	uint16_t  r, g, b, c; 		//valeur lue par le capteur (doit rester en uint)
	int rM=0, gM=0, bM=0, cM=0;	//valeur de RGB moyenne lue

	const int R=1, G=2, B=3, C=4;			// Pour le tableau

	int colorRead[5][taille]; 	 // Tableau des données lues par le capteur pour calculer la moyenne


	for (int i=0;i<taille;i++){
		tcs.getRawData(&r, &g, &b, &c);
		colorRead[R][i]=r;
		colorRead[G][i]=g;
		colorRead[B][i]=b;
		colorRead[C][i]=c;
		
		delay(1);					
	}

	for(int i=0;i<taille;i++){		// Calcul de la moyenne part 1

		rM=rM + colorRead[R][i];
		gM=gM + colorRead[G][i];
		bM=bM + colorRead[B][i];
		cM=cM + colorRead[C][i];
	}

		
		rM = rM / taille; 	  		// Calcul de la moyenne part 2
		gM = gM / taille;
		bM = bM / taille;
		cM = cM / taille;

	if(rM > 900 && gM > 900 && bM > 900) 		//logique de Barin de détection des couleurs
		return BLANC;
		//Serial.println("BLANC");
	else if(rM > gM && rM > bM)
		return ROUGE;
	// Serial.println("ROUGE");
	else if(gM > rM && gM > bM)
		return VERT;
	// Serial.println("VERT");
	else if(bM > rM && bM > gM)
		return BLEU;
	// Serial.println("BLEU");
	else if(rM > bM+300 && gM > bM+300)
		return JAUNE;
	// Serial.println("JAUNE");
	else
		return -1;
	//  Serial.println("ERROR");


}


void loop() {

}