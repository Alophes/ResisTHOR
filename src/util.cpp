#include "util.h"
#include <stdio.h>

void readPulse(){

  pulse->right=pulse->right;
  pulse->left=pulse->left;
  pulse->right=ENCODER_Read(baseSet.ENCODER_RIGHT);
  pulse->left=ENCODER_Read(baseSet.ENCODER_LEFT);
}

void detecteurProximite(){

	if(digitalRead(pin.capDroite)==LOW){ //Détection à droite
		digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
		state->detectRight = 1;
    } 

	else {
		digitalWrite(pin.led_capDroite, HIGH); // Fermeture du led droit
    	state->detectRight = 0;
	}
  
	if(digitalRead(pin.capGauche)==LOW){ //Détection à gauche
    	digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
      	state->detectLeft = 1;
    }
  	else {
		digitalWrite(pin.led_capGauche, HIGH);  // Fermeture du led gauche
    	state->detectLeft = 0;
	}

}

void printState(){

  
  Serial.print("| pulse droit = ");
  Serial.print(pulse->right);
  Serial.print(" |\n");
  Serial.print("| pulse gauche = ");
  Serial.print(pulse->left);
  Serial.print(" |\n");
  Serial.print("| vitesse droite = ");
  Serial.print(speed->forwardRight, 6);
  Serial.print(" |\n");
  Serial.print("| vitesse gauche = ");
  Serial.print(speed->forwardLeft, 6);
  Serial.print(" |\n");
  Serial.print("| detect droit = ");
  Serial.print(state->detectRight);
  Serial.print(" |\n");
  Serial.print("| detect gauche = ");
  Serial.print(state->detectLeft);
  Serial.print(" |\n");
  Serial.print("| moving = ");
  Serial.print(state->moving);
  Serial.print(" |\n");

}

float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }