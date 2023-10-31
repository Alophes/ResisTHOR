#include "util.h"
#include "math.h"

int CALIBRATEMOTORS = 0;
#define AccCALIBRATION 1
#define ForCALIBRATION 1
#define DecCALIBRATION 1

//couleur
#define BLEU 1
#define VERT 2
#define JAUNE 3
#define ROUGE 4

//intégration des librairies
BasicSettings baseSet;
Pin pin;
Color color;
void initColor();
void getColorData();
void printColorData();
void setSeenColor();
void forward(); 
void goForward();//PID et avancer le robot
void stopMotors();
void motorsAccelerate(); // accélération du robot
void readPulse(); // lit les pulses
void printState(); // affiche les données 
State *initState(); // isation des états
Pulse *initPulse(); // isation des pulses
Speed *initSpeed();
void motorCalibration(); // calibration des moteurs
void accCalibration();
void forwardCalibration();
void decelatationCalibration();
int detecteurCouleur();
void detecteurProximite();
int stoppingCriteria();


//detecteur de sifflet
float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }






//isation des variable de base
Speed *speed = initSpeed();
Speed *initialSpeed = initSpeed();
State *state = initState();
Pulse *pulse = initPulse();



void setup() {


	
	BoardInit();
	initColor();
	//detecteur de proximité
	pinMode(pin.capGauche, INPUT); //Pin.capDroite
	pinMode(pin.capGauche, INPUT); //capGauche
	pinMode(pin.led_capDroite, OUTPUT); //ledPin.capDroite
	pinMode(pin.led_capGauche, OUTPUT); //ledcapGauche

	//detecteur de sifflet
	pinMode(PINA0, INPUT);
	pinMode(PINA1, INPUT);

}

void initColor(){
	if (color.tcs.begin()) {
		Serial.println("Found sensor");
	} else {
		Serial.println("No TCS34725 found ... check your connections");
		while (1);
	}
	getColorData();
	setSeenColor();
	color.startColor = color.floorColor;
}
void getColorData(){
	color.tcs.getRawData(&color.r, &color.g, &color.b, &color.c);
	color.colorTemp = color.tcs.calculateColorTemperature(color.r, color.g, color.b);
	color.lux = color.tcs.calculateLux(color.r, color.g, color.b);
}
void printColorData(){
	Serial.print("Color Temp: "); Serial.print(color.colorTemp, DEC); Serial.print(" K - ");
	Serial.print("Lux: "); Serial.print(color.lux, DEC); Serial.print(" - ");
	Serial.print("R: "); Serial.print(color.r, DEC); Serial.print(" ");
	Serial.print("G: "); Serial.print(color.g, DEC); Serial.print(" ");
	Serial.print("B: "); Serial.print(color.b, DEC); Serial.print(" ");
	Serial.print("C: "); Serial.print(color.c, DEC); Serial.print(" ");
	Serial.println(" ");

}
void setSeenColor(){
	if(color.r > 900 && color.g > 900 && color.b > 900){
		Serial.println("BLANC");
		color.floorColor = color.WHITE;
	}
	else if(color.r > color.g && color.r > color.b){
		Serial.println("ROUGE");
		color.floorColor = color.RED;
	}
	else if(color.g > color.r && color.g > color.b){
		Serial.println("VERT");
		color.floorColor = color.GREEN;
	}
	else if(color.b > color.r && color.b > color.g){
		Serial.println("BLEU");
		color.floorColor = color.BLUE;
	}
	else if(color.r > color.b+300 && color.g > color.b+300){
		Serial.println("JAUNE");
		color.floorColor = color.YELLOW;
	}
	else{
		Serial.println("TAPIS");
		color.floorColor = color.CARPET;
	}

}
int stoppingCriteria(){
	/*if(Critère d'arrêt){
		return 1;
	}

	else{
		return 0;
	}*/
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
void forward(){
	state->moving = 1;
	int success = 0;
	// accélération

	detecteurProximite();
	while(state->foward == 1){ //***CONDITION D'ARRET***

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
			if(state->foward == 1){ //***CONDITION D'ARRET***
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

int detecteurCouleur(){
	
}

void actualiseCoordinates(){
  readPulse();


}

Speed *initSpeed(){
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

State *initState(){

  State *etat = (State*)malloc(sizeof(State));

  etat->coordinates[0] = 0;
  etat->coordinates[1] = 0;

  etat->detectLeft = 0;
  etat->detectRight = 0;

  etat->moving = 0;
  etat->foward = 1;

  etat->begin = 1;

  return etat;
}

Pulse *initPulse(){

  Pulse * pul = (Pulse*)malloc(sizeof(Pulse));

  pul->left = 0;
  pul->right = 0;
  
  return pul;

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
			while(1){if(!ROBUS_IsBumper(FRONT)){break;}delay(50);};
		}

		while(state->moving == 1){
			
			getColorData();
			setSeenColor();
			
			if(state->foward == 1){
				motorsAccelerate();
				forward();
				stopMotors();
				delay(50);
			}
			if(/*detecte pas le tape ou le tapis &&*/color.floorColor != color.WHITE){

			}
			else if(/*detecte le tape avec 3 capteur ou le tapis*/1){
				switch (color.startColor)
				{
				case color.GREEN:
					//turn
					break;
				case color.YELLOW:
					//turn
					break;
				}
				
			}
			else if(color.floorColor == color.WHITE){
				while((/*milieu detect pas tape*/1)){ //***CONDITION D'ARRET***
					goForward();
				}
				
			}
		}
		delay(50);
	}

	if(CALIBRATEMOTORS == 1){
		motorCalibration();
		CALIBRATEMOTORS = 0;
	}

}