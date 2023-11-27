#include <stdio.h>
#include <Arduino.h>
#include <Adafruit_TCS34725.h>
#include <LibRobus.h>

//couleur
#define BLEU 1
#define NOIR 2
#define BACHE 3
#define ROUGE 4


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
		

		for(int i=0;i<taille;i++){
				tcs.getRawData(&r, &g, &b, &c);
				
				// delay(1000);

			if(c>80){				//logique de Barin de détection des couleurs
				colorRead[i]=BACHE;
				// Serial.println("BACHE");

				}else if(r > g && r > b && c<75){
					colorRead[i]=ROUGE;
					// Serial.println("Rouge");

						}else if(c<50 && r<20 && g<20 && b<20){
							colorRead[i]=NOIR;
							// Serial.println("NOIR");

							}else if(b > r && b > g){
								colorRead[i]=BLEU;
								// Serial.println("Bleu");

								}else {
										colorRead[i]=-1;
										Serial.println("Donnée de merde");
									}	
									
		}
		int BACHECOUNT=0, RED=0, BLACK=0, BLUE=0;		//Les valeurs seront utilisé pour compter le nombre de fois qu'il a lu la couleur
		int error=0;										//Est pour géré les erreurs
		for(int j=0;j<taille;j++){

			switch (colorRead[j])							//La switch compte le nombre d'occurence d'une couleur
			{
			case BACHE:
				BACHECOUNT=BACHECOUNT + 1;
				break;
			case ROUGE:
				RED=RED + 1;
				break;
			case NOIR:
				BLACK=BLACK + 1;
				break;
			case BLEU:
				BLUE=BLUE + 1;
				break;
			default:
				error=1;
				break;
			}
		}
		if(error==1){								//Si il y assez d'occurence, return la couleur comme valeur, sinon la boucle while recommence
			}else	if(BACHECOUNT>=valIdentique){
						// Serial.println("BACHE");
						return BACHE;

					
					}else	if(RED>=valIdentique){
								// Serial.println("Rouge");
								return ROUGE;

							}else	if(BLACK>=valIdentique){
										// Serial.println("NOIR");
										return NOIR;
									}else	if(BLUE>=valIdentique){
												// Serial.println("Bleu");
												return BLEU;					
											}								
											
											Serial.println("Recommance la lectrue");

	}
}


void loop() {
	uint16_t  r, g, b, c;
	Serial.println("R\tG\tB\tC");
while(1){

	tcs.getRawData(&r, &g, &b, &c);

	if(ROBUS_IsBumper(2)==1){

		// Serial.print(r);
		
		// Serial.print("\t");

		// Serial.print(g);
		
		// Serial.print("\t");

		// Serial.print(b);
		
		// Serial.print("\t");

		// Serial.println(c);
		
		detectColor();
		
	}
}

}