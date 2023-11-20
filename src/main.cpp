#include <stdio.h>
#include <Arduino.h>
#include <Adafruit_TCS34725.h>
#include <LibRobus.h>

//couleur
#define BLEU 1
#define VERT 2
#define JAUNE 3
#define ROUGE 4
#define BLANC 5

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

void setup() {
	
	BoardInit();

	
	if (tcs.begin()) {
    	Serial.println("Found sensor");
	} else {
    	Serial.println("No TCS34725 found ... check your connections");
    	while (1);
  }

}


int detectColor(){

	const int taille=10;		//Nombre de valeur qu'on utilise pour vérifier
	const int valIdentique=8;
	uint16_t  r, g, b, c; 		//valeur lue par le capteur (doit rester en uint16_t)

		// Pour le tableau

	int colorRead[taille]; 	 // Tableau des données lues par le capteur pour comparer les couleurs

	while(1){
		tcs.getRawData(&r, &g, &b, &c);

		for(int i=0;i<taille;i++){

			if(r > 100 && g > 100 && b > 100){		//logique de Barin de détection des couleurs
				colorRead[i]=BLANC;
				
				}else if(r > g && r > b){
					colorRead[i]=ROUGE;

						}else if(g > r && g > b){
							colorRead[i]=VERT;

							}else if(b > r && b > g){
								colorRead[i]=BLEU;

								}else if(r > b+300 && g > b+300){
									colorRead[i]=JAUNE;
									
									}else{
										colorRead[i]=-1;
									}	
		}
		int WHITE=0, RED=0, GREEN=0, BLUE=0, YELLOW=0;		//Les valeurs seront utilisé pour compter le nombre de fois qu'il a lu la couleur
		int error=0;										//Est pour géré les erreurs
		for(int j=0;j<taille;j++){

			switch (colorRead[j])							//La switch compte le nombre d'occurence d'une couleur
			{
			case BLANC:
				WHITE=WHITE + 1;
				break;
			case ROUGE:
				RED=RED + 1;
				break;
			case VERT:
				GREEN=GREEN + 1;
				break;
			case BLEU:
				BLUE=BLUE + 1;
				break;
			case JAUNE:
				YELLOW=YELLOW + 1;
				break;
			default:
				error=1;
				break;
			}
		}
		if(error==1){								//Si il y assez d'occurence, return la couleur comme valeur, sinon la boucle while recommence
			}else	if(WHITE>=valIdentique){
						// Serial.println("Blanc");
						return BLANC;

					}else	if(RED>=valIdentique){
								// Serial.println("Rouge");
								return ROUGE;

							}else	if(GREEN>=valIdentique){
										// Serial.println("Vert");
										return VERT;
									}else	if(BLUE>=valIdentique){
												// Serial.println("Bleu");
												return BLEU;					
											}else	if(YELLOW>=valIdentique){
														// Serial.println("Jaune");
														return JAUNE;								
											}

	}
}


void loop() {

Serial.println("Couleur lue");
Serial.println(detectColor());

delay(2000);

}