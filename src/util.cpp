#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"


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

    Serial.println("Entrez vos valeurs");
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
    delay(1000);
}

void moving(int movement[100], int scAnswer[5], AllStruct allstruct){

    Serial.println("=========================MOVING BEGIN=========================");
    Pin pin = allstruct.pin;


    int nbOfScan = 0;
    int i = 0;
    while(movement[i] != '\0')
    {
        Serial.print("movement [");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(movement[i]);

        if(movement[i] == FORWARD){
            Serial.println("I'm going forward");
            motorsAccelerate(allstruct);
            forward(allstruct);
            stopMotors(allstruct);
        }
        if(movement[i] == TURNLEFT){
            Serial.println("I'm turning left");
            turn(LEFT, allstruct.pin);
        }
        if(movement[i] == TURNRIGHT){
            Serial.println("I'm turning Right");
            turn(RIGHT, allstruct.pin);
        }
        if(movement[i] == SCAN){
            //scAnswer[nbOfScan] = scan();
            //nbOfScan++;
            digitalWrite(pin.ledScan, HIGH);
            delay(500);
            digitalWrite(pin.ledScan, LOW);
        }
        
        i++;
    }
    Serial.println("=========================MOVING END=========================");
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

void returnToBase(int movement[100], int scAnswer[5], AllStruct allstruct)
{

    Serial.println("=========================COMING_BACK BEGIN=========================");
    int i = 0;

    turn(RIGHT, allstruct.pin);
    turn(RIGHT, allstruct.pin);

    while(movement[i] != '\0'){i++;};

    i-=2;
    Serial.print("Last i = ");
    Serial.println(i);

    while(1){

        Serial.print("movement [");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(movement[i]);
    
        if(movement[i] == FORWARD){
            Serial.println("I'm going forward");
            motorsAccelerate(allstruct);
            forward(allstruct);
            stopMotors(allstruct);
        }
        if(movement[i] == TURNLEFT){
            Serial.println("I'm turning right");
            turn(RIGHT, allstruct.pin);
        }
        if(movement[i] == TURNRIGHT){
            Serial.println("I'm turning left");
            turn(LEFT, allstruct.pin);
        }
        

        if(i == 0){
            break;
        }
        i--;
    }

    turn(RIGHT, allstruct.pin);
    turn(RIGHT, allstruct.pin);

    Serial.println("=========================COMING_BACK END=========================");

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


