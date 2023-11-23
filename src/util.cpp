#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X); //le truc a cedk

int color(){

    
	const int taille=10;		//Nombre de valeur qu'on utilise pour vérifier
	const int valIdentique=8;
	uint16_t  r, g, b, c; 		//valeur lue par le capteur (doit rester en uint16_t)

		// Pour le tableau

	int colorRead[taille]; 	 // Tableau des données lues par le capteur pour comparer les couleurs

	while(1){
		

		for(int i=0;i<taille;i++){
				tcs.getRawData(&r, &g, &b, &c);
				
				// delay(1000);

			if(r > 100 && g > 100 && b > 100){		//logique de Barin de détection des couleurs
				colorRead[i]=BLANC;
				// Serial.println("Blanc");

				}else if(r > g && r > b){
					colorRead[i]=ROUGE;
					// Serial.println("Rouge");

						}else if(g > r && g > b){
							colorRead[i]=VERT;
							// Serial.println("Vert");

							}else if(b > r && b > g){
								colorRead[i]=BLEU;
								// Serial.println("Bleu");

								}else if(r > b+300 && g > b+300){
									colorRead[i]=JAUNE;
									// Serial.println("Jaune");

									}else{
										colorRead[i]=-1;
										// Serial.println("Donnée de merde");
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
											// Serial.println("Recommance la lectrue");

	}
}

int readRIFD(){
    //rajouter la fonction

    while(1){

        // tant que ya pas lecture rfid, continu

        //if rifd != 0 break

        if(ROBUS_IsBumper(FRONT)){
            while(ROBUS_IsBumper(FRONT)){
                delay(50);
            }
            Serial.println("Forward");
            return FORWARD;
        }
        
        if(ROBUS_IsBumper(REAR)){
            while(ROBUS_IsBumper(REAR)){
                delay(50);
            }
            return SCAN;
        }
        
        if(ROBUS_IsBumper(LEFT)){
            while(ROBUS_IsBumper(LEFT)){
                delay(50);
            }
            return TURNLEFT;
        }
        
        if(ROBUS_IsBumper(RIGHT)){
            while(ROBUS_IsBumper(RIGHT)){
                delay(50);
            }
            return TURNRIGHT;
        }
        delay(50);
    
    }
}

int choseParkour(){
    int puce;
    puce = readRIFD();
    return puce;
}


void readCommand(int movement[100], int nbmovement){

    int i = 0;
    while (1){
        movement[i] = readRIFD();
        if (movement[i] == SCAN) {
            movement[i+1] = '\0';
            break;
        }
        i++;
    }
    nbmovement = i;

    // int j = 0;
    // Serial.print("movement = ");
    // while(movement[j] != '\0'){
    //     Serial.print(movement[j]);
    //     j++;
    // }
    // Serial.print("\n");
}

void moving(int movement[100], int scAnswer, AllStruct allstruct){

    Pin pin = allstruct.pin;


    int nbOfScan = 0;
    int i = 0;
    while(movement[i] != '\0')
    {
        switch(movement[i]){

            case FORWARD:
                Serial.println("I'm going forward");
                motorsAccelerate(allstruct);
                forward(allstruct);
                stopMotors(allstruct);
            case TURNLEFT:
                Serial.println("I'm turning left");
                turn(LEFT);
            case TURNRIGHT:
                Serial.println("I'm turning Right");
                turn(RIGHT);
            case SCAN:
                //scAnswer[nbOfScan] = scan();
                //nbOfScan++;
                scAnswer = color();
                digitalWrite(pin.ledScan, HIGH);
                delay(500);
                digitalWrite(pin.ledScan, LOW);
        }
        i++;
    }
}

void movingback(int movement[100], int nbmovement){
    turnLeft();
    turnLeft();
    int i = 0;
    for (int i = 0; (nbmovement - i) > 0; i++)
    {
        switch(movement[nbmovement - i]){

            case FORWARD:
                forward();
            case TURNLEFT:
                turnRight();
            case TURNRIGHT:
                turnLeft();
            case SCAN:
        }
        i++;
    }
}


int verifieAnswer(int reponse, int nbAnswer, int scAnswers){
    
        if (reponse != scAnswers)
        {
            return 0;
        }
    return 1;
}

void returnToBase()
{
    return;
}

State detecteurProximite(State state, Pin pin){

	if(digitalRead(pin.capDroite)==LOW){ //Détection à droite
		digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
		state.detectRight = 1;
    } 

	else {
		digitalWrite(pin.led_capDroite, HIGH); // Fermeture du led droit
    	state.detectRight = 0;
	}
  
	if(digitalRead(pin.capGauche)==LOW){ //Détection à gauche
    	digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
      	state.detectLeft = 1;
    }
  	else {
		digitalWrite(pin.led_capGauche, HIGH);  // Fermeture du led gauche
    	state.detectLeft = 0;
	}

    return state;

}

