#include <Arduino.h>
#include <LibRobus.h>
#include "util.h"
#include "math.h"

int CALIBRATEMOTORS = 1;
#define AccCALIBRATION 1
#define ForCALIBRATION 1
#define DecCALIBRATION 1


void forward(float acceleration); //PID et avancer le robot
void stopMotors();
void motorsAccelerate(); // accélération du robot
void readPulse(); // lit les pulses
void printState(); // affiche les données 
State *initState(); // isation des états
Pulse *initPulse(); // isation des pulses
Speed *initSpeed();
void motorCalibration(); // calibration des moteurs
void accCalibration(char affichage);
void forwardCalibration(char affichage);
void decelatationCalibration(char affichage);


//detecteur de sifflet
float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }



//intégration des librairies
BasicSettings baseSet;
Pin pin;

//isation des variable de base
Speed *speed = initSpeed();
Speed *initialSpeed = initSpeed();
State *state = initState();
Pulse *pulse = initPulse();



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

void printState(){

  
  Serial.print("| pulse droit = ");
  Serial.print(pulse->right);
  Serial.print(" |\n");
  Serial.print("| pulse gauche = ");
  Serial.print(pulse->left);
  Serial.print(" |\n");
  Serial.print("| vitesse droite = ");
  Serial.print(speed->motorRight);
  Serial.print(" |\n");
  Serial.print("| vitesse gauche = ");
  Serial.print(speed->motorLeft);
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

void readPulse(){

  pulse->right=pulse->right;
  pulse->left=pulse->left;
  pulse->right=ENCODER_Read(baseSet.ENCODER_RIGHT);
  pulse->left=ENCODER_Read(baseSet.ENCODER_LEFT);
}

void motorsAccelerate(){
	if(CALIBRATEMOTORS == 0){
		int delayMs = 100;
		for(int i = 0; i < 10; i++){
			forward(0.10*(i+1));
			delay(delayMs);
		}
	}

	else{
		int delayMs = 100;
		for(int i = 0; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,initialSpeed->motorRightAcc*(0.10*(i+1)));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT,initialSpeed->motorLeftAcc*(0.10*(i+1)));
			delay(delayMs);
		}

	}
}

void forward(float acceleration){
  
  if(pulse->right < pulse->left) {
    //speed->motorRight =(speed->motorRight+((pulse->left-pulse->right)*baseSet.KP));
    speed->motorLeft =(speed->motorLeft-((pulse->left-pulse->right)*baseSet.KP));
    MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,speed->motorRight*acceleration);
    MOTOR_SetSpeed(baseSet.MOTOR_LEFT,speed->motorLeft*acceleration);
  }
  if(pulse->right > pulse->left) {
    speed->motorLeft = (speed->motorLeft+((pulse->right-pulse->left)*baseSet.KP));
    //speed->motorRight = (speed->motorRight-((pulse->right-pulse->left)*baseSet.KP));
    MOTOR_SetSpeed(baseSet.MOTOR_LEFT,speed->motorLeft*acceleration);
    MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,speed->motorRight*acceleration);
  }
  if(pulse->right == pulse->left) {
    MOTOR_SetSpeed(baseSet.MOTOR_LEFT,speed->motorLeft*acceleration*acceleration);
    MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,speed->motorRight*acceleration*acceleration);
  }
  if(pulse->leftPast != pulse->left) {
    Serial.print("LEFT ");
    Serial.println(pulse->left);
  }
  if(pulse->rightPast != pulse->right) {
    Serial.print("RIGHT ");
    Serial.println(pulse->right);
  }
}

void stopMotors(){
  
	state->moving = 0;
	if(CALIBRATEMOTORS == 0){
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->motorRight*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->motorLeft*((9-i)*0.1));
			delay(75);
		}
	}

	else{
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->motorRightDec*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->motorLeftDec*((9-i)*0.1));
			delay(75);
		}

	}

  //ENCODER_Reset(baseSet.ENCODER_LEFT);
  //ENCODER_Reset(baseSet.ENCODER_RIGHT);
  
}

void detecteurProximite(){

	if(digitalRead(pin.capDroite)==HIGH){ //Détection à droite
      digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
      state->detectRight = 1;
    } 
  else {
		digitalWrite(pin.led_capDroite, LOW); // Fermeture du led droit
    state->detectRight = 0;
	}
  
	if(digitalRead(pin.capGauche)==HIGH){ //Détection à gauche
    	digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
      state->detectLeft = 1;
    }
  else {
		digitalWrite(pin.led_capGauche, LOW);  // Fermeture du led gauche
    state->detectLeft = 0;
	}

}

void actualiseCoordinates(){
  readPulse();


}

Speed *initSpeed(){
	Speed *vitesse = (Speed*)malloc(sizeof(Speed));
	
	vitesse->motorLeft = baseSet.speed_iniL;
	vitesse->motorRight = baseSet.speed_iniR;
	vitesse->motorLeftAcc = baseSet.speed_iniAccL;
	vitesse->motorRightAcc = baseSet.speed_iniAccR;
	vitesse->motorLeftDec = baseSet.speed_iniDecL;
	vitesse->motorRightDec = baseSet.speed_iniDecR;

	return vitesse;
}

State *initState(){

  State *etat = (State*)malloc(sizeof(State));

  etat->coordinates[0] = 0;
  etat->coordinates[1] = 0;

  etat->detectLeft = 0;
  etat->detectRight = 0;

  etat->moving = 0;

  return etat;
}

Pulse *initPulse(){

  Pulse * pul = (Pulse*)malloc(sizeof(Pulse));

  pul->left = 0;
  pul->right = 0;
  
  return pul;

}

void accCalibration(char affichage){
	

	int success = 0;
	// accélération


	while(1){

		ENCODER_Reset(0);
		ENCODER_Reset(1);
		if(affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}
		
		if(success == 5){
			break;
		}
		

		motorsAccelerate();
		readPulse();

		if((pulse->left - pulse->right) > 10 || (pulse->left - pulse->right) < -10){


			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->motorLeftAcc =(initialSpeed->motorLeftAcc-((pulse->left-pulse->right)*baseSet.AccKP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeftAcc = (initialSpeed->motorLeftAcc+((pulse->right-pulse->left)*baseSet.AccKP));
			}

			success = 0;
		}

		else{
			success += 1;
		}

		if(affichage == 'Y'){
			Serial.print("Pulse droit = ");
			Serial.println(pulse->right);
			Serial.print("Pulse gauche = ");
			Serial.println(pulse->left);
			Serial.print("vitesse droite = ");
			Serial.println(initialSpeed->motorRightAcc, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(initialSpeed->motorLeftAcc, 6);
		}
		stopMotors();
		delay(100);

	}

}

void forwardCalibration(char affichage){
	int success = 0;
	// accélération

	motorsAccelerate();

	while(1){
		if(affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 3){
			break;
		}
		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->motorRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->motorLeft);

		delay((success+1)*250);

		readPulse();

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->motorLeft =(initialSpeed->motorLeft-((pulse->left-pulse->right)*baseSet.KP)*(1/pow(2, success)));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeft = (initialSpeed->motorLeft+((pulse->right-pulse->left)*baseSet.KP)*(1/pow(2, success)));
			}

			success = 0;
			
		}

		if(pulse->left == pulse->right){
			success += 1;
		}

		if(affichage == 'Y'){
		Serial.print("pulse gauche - droit = ");
		Serial.println(pulse->left-pulse->right);
		Serial.print("vitesse droite = ");
		Serial.println(initialSpeed->motorRight, 6);
		Serial.print("vitesse gauche = ");
		Serial.println(initialSpeed->motorLeft, 6);
		}

	}

	stopMotors();

}

void decelatationCalibration(char affichage){
	int success = 0;



	while(1){
		if(affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		if(success == 5){
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
				initialSpeed->motorLeftDec =(initialSpeed->motorLeftDec-((pulse->left-pulse->right)*baseSet.KP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeftDec = (initialSpeed->motorLeftDec+((pulse->right-pulse->left)*baseSet.KP));
			}

			success = 0;
		}

		else{
			success += 1;
		}

		if(affichage == 'Y'){
			Serial.print("Pulse droit = ");
			Serial.println(pulse->right);
			Serial.print("Pulse gauche = ");
			Serial.println(pulse->left);
			Serial.print("vitesse droite = ");
			Serial.println(initialSpeed->motorRightDec, 6);
			Serial.print("vitesse gauche = ");
			Serial.println(initialSpeed->motorLeftDec, 6);
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
	Serial.println("Voulez-vous afficher les valeurs des pulses et des vitesses?");
	Serial.println("| Yes : front bumper | No : rear bumper |");
	char affichage;

	while(1){

		if(ROBUS_IsBumper(2)){
			affichage = 'Y';
			break;
		}

		if(ROBUS_IsBumper(3)){
			affichage = 'N';
			break;
		}
		delay(50);
	}



	if(AccCALIBRATION){
		Serial.print("acceleration calibration : ");

		accCalibration(affichage);

		Serial.print("SUCCESS\n");
	}

	if(ForCALIBRATION){
		Serial.print("forward calibration : ");

		forwardCalibration(affichage);

		Serial.print("SUCCESS\n");
	}
	
	if(DecCALIBRATION){
		Serial.print("deceleration calibration : ");

		decelatationCalibration(affichage);

		Serial.print("SUCCESS\n");

	}
	delay(500);


	Serial.print("___________________________________________________________\n");
	Serial.print("||  accélération speed for right motor = ");
	Serial.print(initialSpeed->motorRightAcc, 6);
	Serial.print(" ||\n");
	Serial.print("||  accélération speed for left motor = ");
	Serial.print(initialSpeed->motorLeftAcc, 6);
	Serial.print(" ||\n");

	Serial.print("||  forward speed for right motor = ");
	Serial.print(initialSpeed->motorRight, 6);
	Serial.print(" ||\n");
	Serial.print("||  forward speed for left motor = ");
	Serial.print(initialSpeed->motorLeft, 6);
	Serial.print(" ||\n");


	Serial.print("||  decelaration speed for right motor = ");
	Serial.print(initialSpeed->motorRightDec, 6);
	Serial.print(" ||\n");
	Serial.print("||  decelaration speed for left motor = ");
	Serial.print(initialSpeed->motorLeftDec, 6);
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

		speed->motorLeft = initialSpeed->motorLeft;
		speed->motorRight = initialSpeed->motorRight;
		speed->motorLeftAcc = initialSpeed->motorLeftAcc;
		speed->motorRightAcc = initialSpeed->motorRightAcc;
		speed->motorLeftDec = initialSpeed->motorLeftDec;
		speed->motorRightDec = initialSpeed->motorRightDec;
	}

	Serial.print("changes done.\n");
	delay(500);
	Serial.print("Don't forget to change de value in the util.h of the speed_ini to keep the changes after rebooting the robot ;)\n");
	delay(1500);
}


void loop() {

	if(CALIBRATEMOTORS == 0){
		readPulse();
		printState();

		while(state->moving == 0){

		/*if(detectFrequency()){
			motorsAccelerate();
			state->moving=1;
			break;
		}
		*/

		if(ROBUS_IsBumper(FRONT)){
			motorsAccelerate();
			Serial.print("Speed droit = ");
			Serial.println(speed->motorLeft);
			Serial.print("Speed droit = ");
			Serial.println(speed->motorRight);
			state->moving=1;
			break;
		}
		}

		detecteurProximite();

		if(state->detectLeft == 0 || state->detectRight == 0){
		stopMotors();
		state->moving=0;
		}

		else{
		forward(1);
		}

		delay(50);
	}

	if(CALIBRATEMOTORS == 1){
		motorCalibration();
		CALIBRATEMOTORS = 0;
	}

}