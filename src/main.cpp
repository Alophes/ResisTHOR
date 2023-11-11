#include <Arduino.h>
#include <LibRobus.h>
#include "util.h"
#include "math.h"


void setup() {


	
	BoardInit();

	//detecteur de proximité
	pinMode(pin.capGauche, INPUT); //Pin.capDroite
	pinMode(pin.capGauche, INPUT); //capGauche
	pinMode(pin.led_capDroite, OUTPUT); //ledPin.capDroite
	pinMode(pin.led_capGauche, OUTPUT); //ledcapGauche

	//detecteur de sifflet
	pinMode(PINA0, INPUT);
	pinMode(PINA1, INPUT);

}




void loop() {

	if(state->begin == 1){
		state->begin = 0;
		Serial.println("Voulez-vous afficher les valeurs des pulses et des vitesses?");
		Serial.println("| Yes : front bumper | No : rear bumper |");
		

		while(1){

			if(ROBUS_IsBumper(FRONT)){
				*baseSet.affichage = 'Y';
				while(1){if(!ROBUS_IsBumper(FRONT)){Serial.println("Yes");break;}delay(50);} // pour qu'il break qund le bumper est relaché
				break;
				
			}

			if(ROBUS_IsBumper(REAR)){
				*baseSet.affichage = 'N';
				while(1){if(!ROBUS_IsBumper(REAR)){Serial.println("No");break;}delay(50);}
				break;
			}
			delay(50);
		}
	}

	
	if(CALIBRATEMOTORS == 0){

		if(ROBUS_IsBumper(FRONT)){
			state->moving = 1;
			while(1){if(!ROBUS_IsBumper(FRONT)){break;}delay(50);}; // À effacer si utilise pas le bumper
		}

		while(state->moving == 1){

			motorsAccelerate();
		
			forward(BLEU);

			stopMotors();

			delay(50);
			
		}
		delay(50);
	}

	if(CALIBRATEMOTORS == 1){
		motorCalibration();
		CALIBRATEMOTORS = 0;
	}


}