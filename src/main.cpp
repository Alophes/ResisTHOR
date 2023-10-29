#include <Arduino.h>
#include <LibRobus.h>
#include "util.h"

#define CALIBRATEMOTORS = 1


void forward(float acceleration); //PID et avancer le robot
void stopMotors();
void motorsAccelerate(); // accélération du robot
void readPulse(); // lit les pulses
void printState(); // affiche les données 
State *initState(); // isation des états
Pulse *initPulse(); // isation des pulses
void motorCalibration(); // calibration des moteurs
void accCalibration();
void forwardCalibration();
void decelatationCalibration();


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

  pulse->leftPast=pulse->left;
  pulse->rightPast=pulse->right;
  pulse->left=ENCODER_Read(baseSet.ENCODER_LEFT);
  pulse->right=ENCODER_Read(baseSet.ENCODER_RIGHT);
}

void motorsAccelerate(){
	if(CALIBRATEMOTORS == 0){
		int delayMs = 150;
		for(int i = 0; i < 10; i++){
			forward(0.10*(i+1));
			delay(delayMs);
		}
	}

	else{
		int delayMs = 150;
		for(int i = 0; i < 10; i++){
			MOTOR_SetSpeed(0.10*(i+1));
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT,initialSpeed->motorRight*(0.10*(i+1)));
    		MOTOR_SetSpeed(baseSet.MOTOR_LEFT,initialSpeed->motorLeft*(0.10*(i+1)));
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
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->motorLeft*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->motorRight*((9-i)*0.1));
			delay(50);
		}
	}

	else{
		for(int i = 1; i < 10; i++){
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->motorLeftDec*((9-i)*0.1));
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->motorRightDec*((9-i)*0.1));
			delay(50);
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

void accCalibration(){
	
	int successAcc = 0;
	// accélération

	ENCODER_Reset(0);
	ENCODER_Reset(1);

	while(1){
		if(successAcc == 3){
			break;
		}
		motorsAccelerate();
		readPulse();

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->motorLeftAcc =(initialSpeed->motorLeftAcc-((pulse->left-pulse->right)*baseSet.KP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeftAcc = (initialSpeed->motorLeftAcc+((pulse->right-pulse->left)*baseSet.KP));
			}

			successAcc = 0;
			delay(50);
			continue;
		}

		if(pulse->left == pulse->right){
			successAcc += 1;
		}

		stopMotors();
		delay(50);

	}

}

void forwardCalibration(){
	int success = 0;
	// accélération

	motorsAccelerate();

	while(1){
		if(success == 6){
			break;
		}
		

		ENCODER_Reset(0);
		ENCODER_Reset(1);
	
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->motorLeft);
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->motorRight);

		delay(success * 250);

		readPulse();

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->motorLeft =(initialSpeed->motorLeft-((pulse->left-pulse->right)*baseSet.KP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeft = (initialSpeed->motorLeft+((pulse->right-pulse->left)*baseSet.KP));
			}

			success = 0;
			delay(50);
			continue;
		}

		if(pulse->left == pulse->right){
			success += 1;
		}

		delay(50);

	}

	stopMotors();

}

void decelatationCalibration(){
	int success = 0;
	// accélération

	

	while(1){
		if(success == 3){
			break;
		}
		
		motorsAccelerate();
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, initialSpeed->motorLeft);
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, initialSpeed->motorRight);
		delay(500);
		ENCODER_Reset(0);
		ENCODER_Reset(1);
		
		stopMotors();
		readPulse();

		if(pulse->left != pulse->right && pulse->left != 0 && pulse->left != 0){

			// PID d'ajustement de la roue gauche en fonction de la roue droite 

			if(pulse->right < pulse->left) {
				initialSpeed->motorLeftDec =(initialSpeed->motorLeftDec-((pulse->left-pulse->right)*baseSet.KP));
			}

			if(pulse->right > pulse->left) {
				initialSpeed->motorLeftDec = (initialSpeed->motorLeftDec+((pulse->right-pulse->left)*baseSet.KP));
			}

			success = 0;
			delay(50);
			continue;
		}

		if(pulse->left == pulse->right){
			success += 1;
		}

		delay(50);

	}


}

void motorCalibration(){

	// printf en ver : printf("\033[0;32m")
	system("clear");

	printf("acceleration calibration : ");

	accCalibration();

	printf("\033[0;32m");
	printf("SUCCESS\n");
	printf("\033[0;30m");
	

	printf("forward calibration : ");

	forwardCalibration();

	printf("\033[0;32m");
	printf("SUCCESS\n");
	printf("\033[0;30m");

	printf("deceleration calibration : ");

	decelatationCalibration();

	printf("\033[0;32m");
	printf("SUCCESS\n");
	printf("\033[0;30m");

	delay(500);

	system("clear");

	printf("___________________________________________________________\n");
	printf("||  accélération speed for right motor = %f ||\n", initialSpeed->motorRightAcc);
	printf("||  accélération speed for left motor = %f ||\n", initialSpeed->motorLeftAcc);

	printf("||  forward speed for right motor = %f ||\n", initialSpeed->motorRight);
	printf("||  forward speed for left motor = %f ||\n", initialSpeed->motorLeft);

	printf("||  decelaration speed for right motor = %f ||\n", initialSpeed->motorRightDec);
	printf("||  decelaration speed for left motor = %f ||\n", initialSpeed->motorLeftDec);
	printf("___________________________________________________________\n");

	printf("Do you want de new value to be set in your speed settings?\n");
	printf("(Y/N)\n");

	char response;
	scanf("%c", &response);
	while(response != 'Y' && response != 'N'){
		printf("the answer must be 'Y' or 'N'\n");
		scanf("%c", &response);
	}

	if(response == 'Y'){

		speed->motorLeft = initialSpeed->motorLeft;
		speed->motorRight = initialSpeed->motorRight;
		speed->motorLeftAcc = initialSpeed->motorLeftAcc;
		speed->motorRightAcc = initialSpeed->motorRightAcc;
		speed->motorLeftDec = initialSpeed->motorLeftDec;
		speed->motorRightDec = initialSpeed->motorRightDec;
	}

	system("clear");
	printf("changes done.\n");
	delay(500);
	printf("Don't forget to change de value in the util.h of the speed_ini to keep the changes after rebooting the robot ;)\n");
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
	}

}