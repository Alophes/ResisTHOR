#include "stdio.h"
#include "util.h"
#include <LibRobus.h>
#include <Arduino.h>

#ifndef LCD
#include "lcd.h"
#endif


void motorsAccelerate(AllStruct *allStruct){

    Speed *initialSpeed = allStruct->initialSpeed;
    Speed *speed = allStruct->speed;
    Pulse *pulse = allStruct->pulse;
    BasicSettings baseSet = allStruct->baseSet;
	State *state = allStruct->state;

	if(baseSet.CALIBRATEMOTORS == 0){
		int delayMs = 50;
		for(int i = 0; i < 10; i++){
			detecteurProximite(allStruct->state, allStruct->pin);

			if(state->detectLeft == DETECT){
				return;
			}
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->accelerationRight*0.10*(i+1));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->accelerationLeft*0.10*(i+1));
			delay(delayMs);
		}
	}

	else{
		int delayMs = 100;
		for(int i = 0; i < 10; i++){
			if(state->detectLeft == DETECT){
				return;
			}
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,initialSpeed->accelerationRight*0.10*(i+1));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT,initialSpeed->accelerationLeft*0.10*(i+1));
			delay(delayMs);
		}

	}

}

void forward(AllStruct *allStruct){

    Speed *speed = allStruct->speed;
    Pulse *pulse = allStruct->pulse;
    State *state = allStruct->state;
    ForwardParam forwardParam;
    Pin pin = allStruct->pin;
    BasicSettings baseSet = allStruct->baseSet;

    float potentioMeterForward = (float)analogRead(pin.potentiometerForward)/1023;
    Serial.print("potentioMeter =");
    Serial.println(potentioMeterForward);


	int success = 0;
	// accélération

	detecteurProximite(state, pin);
	for(int i = 0; i < forwardParam.nbIteration*potentioMeterForward; i++){ 
        
        //CONDITION D'ARRET
        /*if(stoppingCriteria(allStruct) == 1){
			printLCD(SADFACE, allStruct);
            delay(forwardParam.breakDelay);
        }*/
		if(baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);

		for(int i = 0 ; i <= 4; i++){ // ici c'est pour mettre un delay(250) en s'assurant qu'il vérifie quand meme detecteurProx

			detecteurProximite(state, pin);
			/*if(stoppingCriteria(allStruct) == 1){ //CONDITION D'ARRET
				return;
			}*/
            delay(forwardParam.delayIteration);
		}


		readPulse(allStruct);

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){


			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				speed->forwardLeft=(speed->forwardRight-((pulse->left-pulse->right)*baseSet.KP)*(1/pow(2, success)));
			}

			if(pulse->right > pulse->left) {
				speed->forwardLeft= (speed->forwardLeft+((pulse->right-pulse->left)*baseSet.KP)*(1/pow(2, success)));
			}

			//success = 0;
			
		}

		if(pulse->left == pulse->right){
			success += 1;
		}

		if(baseSet.affichage == 'Y'){
			Serial.print("pulse gauche - droit = ");
			Serial.println(pulse->left-pulse->right);
			Serial.print("vitesse droite = ");
			Serial.println(speed->forwardRight, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(speed->forwardLeft, 6);
		}

		detecteurProximite(state, pin);
        delay(forwardParam.delayIteration);
	}

}

void stopMotors(AllStruct *allStruct){
    
    BasicSettings baseSet = allStruct->baseSet;

    Speed *initialSpeed = allStruct->initialSpeed;
    Speed *speed = allStruct->speed;


	if(baseSet.CALIBRATEMOTORS == 0){
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->decelerationRight*(9-i)*0.1);
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->decelerationLeft*(9-i)*0.1);
			delay(25);
		}
	}

	else{
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->decelerationRight*(9-i)*0.1);
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->decelerationLeft*(9-i)*0.1);
			delay(25);
		}

	}

  //ENCODER_Reset(baseSet.ENCODER_LEFT);
  //ENCODER_Reset(baseSet.ENCODER_RIGHT);

  
}

void accCalibration(AllStruct *allStruct){
	
    Speed *initialSpeed = allStruct->initialSpeed;
    Pulse *pulse = allStruct->pulse;
    BasicSettings baseSet = allStruct->baseSet;

	int success = 0;
	// accélération


	while(1){

		ENCODER_Reset(0);
		ENCODER_Reset(1);
		if(baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}
		
		if(success == 5){
			break;
		}
		

		motorsAccelerate(allStruct);
		readPulse(allStruct);

		if((pulse->left - pulse->right) > 3 || (pulse->left - pulse->right) < -10){


			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->accelerationLeft =(initialSpeed->accelerationLeft-((pulse->left-pulse->right)*baseSet.AccKP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->accelerationLeft = (initialSpeed->accelerationLeft+((pulse->right-pulse->left)*baseSet.AccKP));
			}

			success = 0;
		}

		else{
			success += 1;
		}

		if(baseSet.affichage == 'Y'){
			Serial.print("Pulse droit = ");
			Serial.println(pulse->right);
			Serial.print("Pulse gauche = ");
			Serial.println(pulse->left);
			Serial.print("vitesse droite = ");
			Serial.println(initialSpeed->accelerationRight, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(initialSpeed->accelerationLeft, 6);
		}
		stopMotors(allStruct);
		delay(100);

	}

}

void forwardCalibration(AllStruct *allStruct){

    Speed *initialSpeed = allStruct->initialSpeed;
    Pulse *pulse = allStruct->pulse;
    BasicSettings baseSet = allStruct->baseSet;



	int success = 0;
	// accélération

	motorsAccelerate(allStruct);

	while(1){
		if(baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 8){
			break;
		}
		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->forwardRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->forwardLeft);

		delay((success+1)*250);

		readPulse(allStruct);

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->forwardLeft=(initialSpeed->forwardLeft-((pulse->left-pulse->right)*baseSet.KP)*(1/pow(2, success)));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->forwardLeft= (initialSpeed->forwardLeft+((pulse->right-pulse->left)*baseSet.KP)*(1/pow(2, success)));
			}

			//success = 0;
			
		}

		if(pulse->left == pulse->right){
			success += 1;
		}

		if(baseSet.affichage == 'Y'){
		Serial.print("pulse gauche - droit = ");
		Serial.println(pulse->left-pulse->right);
		Serial.print("vitesse droite = ");
		Serial.println(initialSpeed->forwardRight, 6);
		Serial.print("vitesse gauche = ");
		Serial.println(initialSpeed->forwardLeft, 6);
		}

	}

	stopMotors(allStruct);


}

void decelatationCalibration(AllStruct *allStruct){

    Speed *initialSpeed = allStruct->initialSpeed;
    Pulse *pulse = allStruct->pulse;
    BasicSettings baseSet = allStruct->baseSet;



	int success = 0;



	while(1){
		if(baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 3){
			break;
		}
		
		motorsAccelerate(allStruct);

		ENCODER_Reset(0);
		ENCODER_Reset(1);
		
		stopMotors(allStruct);
		readPulse(allStruct);

		if((pulse->left - pulse->right) > 10 || (pulse->left - pulse->right) < -10){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->decelerationLeft =(initialSpeed->decelerationLeft-((pulse->left-pulse->right)*baseSet.KP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->decelerationLeft = (initialSpeed->decelerationLeft+((pulse->right-pulse->left)*baseSet.KP));
			}

			success = 0;
		}

		else{
			success += 1;
		}

		if(baseSet.affichage == 'Y'){
			Serial.print("Pulse droit = ");
			Serial.println(pulse->right);
			Serial.print("Pulse gauche = ");
			Serial.println(pulse->left);
			Serial.print("vitesse droite = ");
			Serial.println(initialSpeed->decelerationRight, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(initialSpeed->decelerationLeft, 6);
		}
		delay(100);
		

	}

}

void readPulse(AllStruct *allStruct){

    Pulse *pulse = allStruct->pulse;
    BasicSettings baseSet = allStruct->baseSet;



    pulse->right=ENCODER_Read(baseSet.ENCODER_RIGHT);
    pulse->left=ENCODER_Read(baseSet.ENCODER_LEFT);

}

void motorCalibration(AllStruct *allStruct){

    Speed *initialSpeed = allStruct->initialSpeed;



	Serial.println("=========================CALIBRATION=========================");
	delay(250);
	Serial.println("Click on the front bumper to start the calibration");
	while(ROBUS_IsBumper(2)==0){
			delay(50);
		}

	delay(100);



	if(AccCALIBRATION){
		Serial.print("acceleration calibration : ");

		accCalibration(allStruct);

		Serial.print("SUCCESS\n");
	}

	if(ForCALIBRATION){
		Serial.print("forward calibration : ");

		forwardCalibration(allStruct);

		Serial.print("SUCCESS\n");
	}
	
	if(DecCALIBRATION){
		Serial.print("deceleration calibration : ");

		decelatationCalibration(allStruct);

		Serial.print("SUCCESS\n");

	}
	delay(500);


	Serial.print("___________________________________________________________\n");
	Serial.print("||  accélération speed for right motor = ");
	Serial.print(initialSpeed->accelerationRight, 6);
	Serial.print(" ||\n");
	Serial.print("||  accélération speed for left motor = ");
	Serial.print(initialSpeed->accelerationLeft, 6);
	Serial.print(" ||\n");

	Serial.print("||  forward speed for right motor = ");
	Serial.print(initialSpeed->forwardRight, 6);
	Serial.print(" ||\n");
	Serial.print("||  forward speed for left motor = ");
	Serial.print(initialSpeed->forwardLeft, 6);
	Serial.print(" ||\n");


	Serial.print("||  decelaration speed for right motor = ");
	Serial.print(initialSpeed->decelerationRight, 6);
	Serial.print(" ||\n");
	Serial.print("||  decelaration speed for left motor = ");
	Serial.print(initialSpeed->decelerationLeft, 6);
	Serial.print(" ||\n");
	Serial.print("___________________________________________________________\n");

	Serial.print("Do you want de new value to be set in your speed settings?\n");
	Serial.println("| Yes : front bumper | No : rear bumper |");

	char response;
	while(1){

		if(ROBUS_IsBumper(2)){
			response = 'Y';
			break;
		}

		if(ROBUS_IsBumper(3)){
			response = 'N';
			break;
		}
		delay(50);
	}
	


	if(response == 'Y'){

        Speed *speed = allStruct->speed;

		speed->forwardLeft= initialSpeed->forwardLeft;
		speed->forwardRight = initialSpeed->forwardRight;
		speed->accelerationLeft = initialSpeed->accelerationLeft;
		speed->accelerationRight = initialSpeed->accelerationRight;
		speed->decelerationLeft = initialSpeed->decelerationLeft;
		speed->decelerationRight = initialSpeed->decelerationRight;
	}

	Serial.print("changes done.\n");
	delay(500);
	Serial.print("Don't forget to change de value in the util.h of the speed_ini to keep the changes after rebooting the robot ;)\n");
	delay(1500);

}

Speed *initSpeed(BasicSettings baseSet){
	Speed *vitesse = (Speed*)malloc(sizeof(Speed));

	
	if(baseSet.robot == 'A'){
		vitesse->forwardLeft = baseSet.speedRobotA.forwardL;
		vitesse->forwardRight = baseSet.speedRobotA.forwardR;
		vitesse->accelerationLeft = baseSet.speedRobotA.accelerationL;
		vitesse->accelerationRight = baseSet.speedRobotA.accelerationR;
		vitesse->decelerationLeft = baseSet.speedRobotA.decelerationL;
		vitesse->decelerationRight = baseSet.speedRobotA.decelerationR;
	}
	
	if(baseSet.robot == 'B'){
		vitesse->forwardLeft = baseSet.speedRobotB.forwardL;
		vitesse->forwardRight = baseSet.speedRobotB.forwardR;
		vitesse->accelerationLeft = baseSet.speedRobotB.accelerationL;
		vitesse->accelerationRight = baseSet.speedRobotB.accelerationR;
		vitesse->decelerationLeft = baseSet.speedRobotB.decelerationL;
		vitesse->decelerationRight = baseSet.speedRobotB.decelerationR;
	}
	return vitesse;
}

Pulse *initPulse(){

  Pulse *pul = (Pulse*)malloc(sizeof(Pulse));

  pul->left = 0;
  pul->right = 0;
  
  return pul;

}

State *initState(){

    State *etat = (State*)malloc(sizeof(State));

    etat->start = 1;
	etat->comingBack = 0;

    return etat;
}

AllStruct *initAllStruct(BasicSettings baseSet, Pin pin){
    AllStruct *allstruc = (AllStruct*)malloc(sizeof(AllStruct));

    allstruc->baseSet = baseSet;
    allstruc->pin = pin;

    allstruc->pulse = initPulse();
    allstruc->initialSpeed = initSpeed(baseSet);
    allstruc->speed = initSpeed(baseSet);
    allstruc->state = initState();
    return allstruc;


}

void turn(int direction, Pin pin){

    Serial.println("=========================TURNING_BEGIN=========================");

    float potentiometerTurnRight = (float)analogRead(pin.potentiometerTurnRight)/1023;
	float potentiometerTurnLeft = (float)analogRead(pin.potentiometerTurnLeft)/1023;
    Serial.print("potentioMeterRight =");
    Serial.println(potentiometerTurnRight);
	Serial.print("potentioMeterRight =");
    Serial.println(potentiometerTurnRight);
	MOTOR_SetSpeed(0,0);
	MOTOR_SetSpeed(1,0);


	int PULSES_PAR_TOUR = 3200;
	float DIAMETRE_ROUE = 7.62;
	float DistanceEntreRoue=19.5;

	ENCODER_Reset(1);
	ENCODER_Reset(0); // Encodeur gauche

		
	// Calculer la distance à parcourir pour tourner de 90 degrés
	float circonferenceRoue = PI * DIAMETRE_ROUE;
	float DistanceARouler=DistanceEntreRoue*2.0*PI*0.125;

    int motorStopped = 2;

    if(direction==LEFT){
		while(1)
		{
            if(ENCODER_Read(0)>((-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR))*(1-0.1*potentiometerTurnLeft)){
                MOTOR_SetSpeed(0 , -0.2);
            }
			
            else{
                MOTOR_SetSpeed(0 , 0);
                motorStopped--;
            }

            if(ENCODER_Read(1)<((DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR))*(1-0.1*potentiometerTurnLeft)){
                MOTOR_SetSpeed(1 , 0.2);
            }
			
            else{
                MOTOR_SetSpeed(1 , 0);
                motorStopped--;
            }

            if(motorStopped <= 0){
                break;
            }

            delay(20);



			
		}

	}

	if(direction==RIGHT){

        while(1)
		{
            if(ENCODER_Read(0)<((DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)*(1-0.1*potentiometerTurnRight))){
                MOTOR_SetSpeed(0 , 0.2);
            }
			
            else{
                MOTOR_SetSpeed(0 , 0);
                motorStopped--;
            }

            if(ENCODER_Read(1)>((-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)*(1-0.1*potentiometerTurnRight))){
                MOTOR_SetSpeed(1 , -0.2);
            }
			
            else{
                MOTOR_SetSpeed(1 , 0);
                motorStopped--;
            }

            if(motorStopped <= 0){
                break;
            }
            delay(20);

        }
    }
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);

    Serial.println("=========================TURNING_END=========================");
}

void testMovement(AllStruct *allstruct){

    Serial.println("==================TEST MOVEMENT BEGIN===================");
    while(1){

        int mouv = readRIFD();
		delay(500);

        if(mouv == FORWARD){
            motorsAccelerate(allstruct);
            forward(allstruct);
            stopMotors(allstruct);
        }
        
        
        if(mouv == TURNLEFT){
            turn(LEFT, allstruct->pin);
        }
        
        if(mouv == TURNRIGHT){
            turn(RIGHT, allstruct->pin);
        }
        delay(50);
    
    }

    Serial.println("==================TEST MOVEMENT BEGIN===================");

}

