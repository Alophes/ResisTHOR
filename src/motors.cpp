#include "util.h"
#include "stdio.h"


void motorsAccelerate(){
	if(CALIBRATEMOTORS == 0){
		int delayMs = 100;
		for(int i = 0; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,speed->accelerationRight*(0.10*(i+1)));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT,speed->accelerationLeft*(0.10*(i+1)));
			delay(delayMs);
		}

		readPulse();

		if((pulse->left - pulse->right) > 10 || (pulse->left - pulse->right) < -10){


			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				speed->accelerationLeft =(speed->accelerationLeft-((pulse->left-pulse->right)*baseSet.AccKP));
			}

			if(pulse->right > pulse->left) {
				speed->accelerationLeft = (speed->accelerationLeft+((pulse->right-pulse->left)*baseSet.AccKP));
			}

		}
	}

	else{
		int delayMs = 100;
		for(int i = 0; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,initialSpeed->accelerationRight*(0.10*(i+1)));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT,initialSpeed->accelerationLeft*(0.10*(i+1)));
			delay(delayMs);
		}

	}
}

void forward(int colorToFollow){
	state->moving = 1;
	int success = 0;
	// accélération

	detecteurProximite();
	while(state->detectLeft == 0 && state->detectRight == 0){ //***CONDITION D'ARRET***

		if(*baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);

		for(int i = 0 ; i <= ((success+1)*5) ; i++){ // ici c'est pour mettre un delay(250) en s'assurant qu'il vérifie quand meme detecteurProx

			detecteurProximite();
			if(state->detectLeft == 1 || state->detectRight == 1){ //***CONDITION D'ARRET***
				return;
			}
		}


		readPulse();

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

		if(*baseSet.affichage == 'Y'){
			Serial.print("pulse gauche - droit = ");
			Serial.println(pulse->left-pulse->right);
			Serial.print("vitesse droite = ");
			Serial.println(speed->forwardRight, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(speed->forwardLeft, 6);
		}

		detecteurProximite();

	}
}

void stopMotors(){
  
	state->moving = 0;
	if(CALIBRATEMOTORS == 0){
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->decelerationRight*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->decelerationLeft*((9-i)*0.1));
			delay(75);
		}
	}

	else{
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->decelerationRight*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->decelerationLeft*((9-i)*0.1));
			delay(75);
		}

	}

  //ENCODER_Reset(baseSet.ENCODER_LEFT);
  //ENCODER_Reset(baseSet.ENCODER_RIGHT);
  
}

void accCalibration(){
	

	int success = 0;
	// accélération


	while(1){

		ENCODER_Reset(0);
		ENCODER_Reset(1);
		if(*baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}
		
		if(success == 3){
			break;
		}
		

		motorsAccelerate();
		readPulse();

		if((pulse->left - pulse->right) > 10 || (pulse->left - pulse->right) < -10){


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

		if(*baseSet.affichage == 'Y'){
			Serial.print("Pulse droit = ");
			Serial.println(pulse->right);
			Serial.print("Pulse gauche = ");
			Serial.println(pulse->left);
			Serial.print("vitesse droite = ");
			Serial.println(initialSpeed->accelerationRight, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(initialSpeed->accelerationLeft, 6);
		}
		stopMotors();
		delay(100);

	}

}

void forwardCalibration(){
	int success = 0;
	// accélération

	motorsAccelerate();

	while(1){
		if(*baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 3){
			break;
		}
		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->forwardRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->forwardLeft);

		delay((success+1)*250);

		readPulse();

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

		if(*baseSet.affichage == 'Y'){
		Serial.print("pulse gauche - droit = ");
		Serial.println(pulse->left-pulse->right);
		Serial.print("vitesse droite = ");
		Serial.println(initialSpeed->forwardRight, 6);
		Serial.print("vitesse gauche = ");
		Serial.println(initialSpeed->forwardLeft, 6);
		}

	}

	stopMotors();

}

void decelatationCalibration(){
	int success = 0;



	while(1){
		if(*baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 3){
			break;
		}
		
		motorsAccelerate();

		ENCODER_Reset(0);
		ENCODER_Reset(1);
		
		stopMotors();
		readPulse();

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

		if(*baseSet.affichage == 'Y'){
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

void motorCalibration(){


	Serial.println("=========================CALIBRATION=========================");
	delay(250);
	Serial.println("Click on the front bumper to start the calibration");
	while(ROBUS_IsBumper(2)==0){
			delay(50);
		}

	delay(100);



	if(AccCALIBRATION){
		Serial.print("acceleration calibration : ");

		accCalibration();

		Serial.print("SUCCESS\n");
	}

	if(ForCALIBRATION){
		Serial.print("forward calibration : ");

		forwardCalibration();

		Serial.print("SUCCESS\n");
	}
	
	if(DecCALIBRATION){
		Serial.print("deceleration calibration : ");

		decelatationCalibration();

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