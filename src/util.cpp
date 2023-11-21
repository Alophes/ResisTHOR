#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"

Pin pin;


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

void readCommand(int movement[100]){
    int i = 0;
    while (1){
        movement[i] = readRIFD();
        if (movement[i] == SCAN) {
            movement[i+1] = '\0';
            break;
        }
        i++;
    }

    int j = 0;
    Serial.print("movement = ");
    while(movement[j] != '\0'){
        Serial.print(movement[j]);
        j++;
    }
    Serial.print("\n");
}

void moving(int movement[100], int scAnswer[5], AllStruct allstruct){

    int nbOfScan = 0;
    int i = 0;
    while(movement[i] != '\0')
    {
        switch(movement[i]){

            case FORWARD:
                motorsAccelerate(allstruct);
                forward(allstruct);
                stopMotors(allstruct);
            case TURNLEFT:
                turn(LEFT);
            case TURNRIGHT:
                turn(RIGHT);
            case SCAN:
                scAnswer[nbOfScan] = scan();
                nbOfScan++;
        }
        i++;
    }
}



int verifieAnswer(int reponse[5], int nbAnswer, int scAnswers[5]){
    for (int i =0; i <= nbAnswer; i++)
    {
        if (reponse[5] != scAnswers[5])
        {
            return 0;
        }
    }
    return 1;
}

void returnToBase()
{
    return;
}

State detecteurProximite(State state){

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

