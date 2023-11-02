#include <stdio.h>
#include "util.h"
#include "math.h"
#include <Arduino.h>
#include "Adafruit_TCS34725.h"



#define MAX_RIGHTlarge 12800
#define MAX_LEFTlarge 18560

#define MAX_RIGHTshort 8827
#define MAX_LEFTshort 12800

int distanceShortCut = 0;
#define NB_SECTIONS 10
int CALIBRATEMOTORS = 0;
#define AccCALIBRATION 1
#define ForCALIBRATION 1
#define DecCALIBRATION 1

//couleur
#define BLEU 1
#define VERT 2
#define JAUNE 3
#define ROUGE 4

#define TEST 1
#define TEST_followTheLine 0
#define TEST_detectColor 0
#define TEST_turn 0

#define HARDCODE 1

#define RIGHT 1
#define LEFT 0
//intégration des librairies
BasicSettings baseSet;
Pin pin;
Color color;
void initColor();
void getColorData();
void printColorData();
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
bool stoppingCriteria();
void largeTurn();
void followLine();
void turn(int direction);
void turnAngle(int direction, int angle);
void updateDetectLine();
void dropTheCup ();
void calibrateColor();
float convertDistanceIR(uint8_t id);


//detecteur de sifflet
float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }






//isation des variable de base
Speed *speed = initSpeed();
Speed *initialSpeed = initSpeed();
State *state = initState();
Pulse *pulse = initPulse();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);
uint16_t r, g, b, c, colorTemp, lux;


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

	pinMode(pin.lineDetectL, INPUT);
    pinMode(pin.lineDetectM, INPUT);
    pinMode(pin.lineDetectR, INPUT);

	SERVO_Enable(1);
	SERVO_SetAngle(1,0);
	SERVO_Enable(SERVO_1);
	SERVO_SetAngle(0, 0);



}

/**
 * @brief Convertit la valeur IR en distance en centimètres.
 * @param id L'identifiant du capteur IR (0: gauche, 1:droit, 2:avant, 3:arrière)
 * @return La distance en centimètres.
 */
float convertDistanceIR(uint8_t id){

  float InfraredVal = ROBUS_ReadIR(id);

  float analogInVoltage = InfraredVal * (5.0 / 1023);
  
  float distanceCM = 0;

  distanceCM = 29.988 * pow(analogInVoltage, -1.173);

  return distanceCM;
}


void initColor(){
	
	while(1){
		if (tcs.begin()) {
			Serial.println("Found sensor");
			break;
		} else {
			Serial.println("No TCS34725 found ... check your connections");
		}
		delay(50);
	}
	getColorData();
	color.startColor = color.floorColor;
}

void updateDetectLine(){

	state->lineDetectL = digitalRead(pin.lineDetectL);
    state->lineDetectR = digitalRead(pin.lineDetectR);
    state->lineDetectM = digitalRead(pin.lineDetectM);

}

void printColorData(){
	Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
	Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
	Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
	Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
	Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
	Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
	Serial.println(" ");

}

void getColorData(){

	tcs.getRawData(&r, &g, &b, &c);

	
	Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
	Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
	Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
	Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
	Serial.println(" ");


	if(/*r > 300 && g > 300 && b > 300*/ c > 4600) // blanc
		{Serial.println("Blanc");
		color.floorColor = color.WHITE;}

	else if(c > 3000) { //jaune
		Serial.println("jaune");
		color.floorColor = color.YELLOW;
	}

	else if(c > 2100) //Tapis
		{Serial.println("carpet");
		color.floorColor = color.CARPET;}

	else if(r > g && r > b ) // rouge
		{Serial.println("rouge");
		color.floorColor = color.RED;}

	else if(g > r && g > b) // vert
		{Serial.println("vert");
		color.floorColor = color.GREEN;}

	else if(b > r && b > g) // bleu
		{Serial.println("bleu");
		color.floorColor = color.BLUE;}


}

void turn(int direction){

	MOTOR_SetSpeed(0,0);
	MOTOR_SetSpeed(1,0);
	if(direction==LEFT){

	int PULSES_PAR_TOUR = 3200;
	float DIAMETRE_ROUE = 7.62;
	float DistanceEntreRoue=19.5;

	ENCODER_ReadReset(1);
	ENCODER_Reset(0); // Encodeur gauche

		
	// Calculer la distance à parcourir pour tourner de 180 degrés
	float circonferenceRoue = PI * DIAMETRE_ROUE;
	float DistanceARouler=DistanceEntreRoue*2.0*PI*0.125;

		while(ENCODER_Read(0)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)+80)
		{
			MOTOR_SetSpeed(0 , -0.2);
			
		}
		MOTOR_SetSpeed(0 , 0);
		delay(100);

		while(ENCODER_Read(1)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)-70)
		{
			MOTOR_SetSpeed(1 , 0.2);
		
		}
		MOTOR_SetSpeed(1 , 0);
		delay(500);

	}

	if(direction==RIGHT){

		int PULSES_PAR_TOUR = 3200;
		float DIAMETRE_ROUE = 7.62;
		float DistanceEntreRoue=19.5;

		ENCODER_ReadReset(1);
		ENCODER_Reset(0); // Encodeur gauche

			
		// Calculer la distance à parcourir pour tourner de 180 degrés
		float circonferenceRoue = PI * DIAMETRE_ROUE;
		float DistanceARouler=DistanceEntreRoue*2.0*PI*0.125;

		while(ENCODER_Read(0)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR-80)){
				MOTOR_SetSpeed(0 , 0.2);

				Serial.print("Encodeur gauche \t");
				Serial.println(ENCODER_Read(0));
				
			}
			MOTOR_SetSpeed(0 , 0);
			delay(100);

		while(ENCODER_Read(1)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR+70))
			{
				MOTOR_SetSpeed(1 , -0.2);

				Serial.print("Encodeur droit \t");
				Serial.println(ENCODER_Read(1));
			
			}
			MOTOR_SetSpeed(1 , 0);

	}
}

void turnAngle(int direction, int angle){

float PortionArcCercle=angle/180.0*0.25;
MOTOR_SetSpeed(0 , 0);
MOTOR_SetSpeed(1 , 0);
delay(100);
if(direction==LEFT){

int PULSES_PAR_TOUR = 3200;
float DIAMETRE_ROUE = 7.62;
float DistanceEntreRoue=19.5;

ENCODER_ReadReset(1);
ENCODER_Reset(0); // Encodeur gauche

    
// Calculer la distance à parcourir pour tourner de 180 degrés
float circonferenceRoue = PI * DIAMETRE_ROUE;
float DistanceARouler=DistanceEntreRoue*2.0*PI*PortionArcCercle;

	while(ENCODER_Read(0)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)+80)
	{
		MOTOR_SetSpeed(0 , -0.2);

	}
	MOTOR_SetSpeed(0 , 0);
	delay(100);

	while(ENCODER_Read(1)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)-70)
	{
		MOTOR_SetSpeed(1 , 0.2);


	}
	MOTOR_SetSpeed(1 , 0);
	delay(500);

}

if(direction==RIGHT){

int PULSES_PAR_TOUR = 3200;
float DIAMETRE_ROUE = 7.62;
float DistanceEntreRoue=19.5;

ENCODER_ReadReset(1);
ENCODER_Reset(0); // Encodeur gauche

    
// Calculer la distance à parcourir pour tourner de 180 degrés
float circonferenceRoue = PI * DIAMETRE_ROUE;
float DistanceARouler=DistanceEntreRoue*2.0*PI*PortionArcCercle;

	while(ENCODER_Read(0)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR-80))
	{
		MOTOR_SetSpeed(0 , 0.2);

		Serial.print("Encodeur gauche \t");
		Serial.println(ENCODER_Read(0));
		
	}
	MOTOR_SetSpeed(0 , 0);
	delay(100);

	while(ENCODER_Read(1)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR+70))
	{
		MOTOR_SetSpeed(1 , -0.2);

		Serial.print("Encodeur droit \t");
		Serial.println(ENCODER_Read(1));
	
	}
	MOTOR_SetSpeed(1 , 0);

}
}

void turn45(int direction){

	MOTOR_SetSpeed(0 , 0);
	MOTOR_SetSpeed(1 , 0);
	delay(100);
	if(direction==LEFT){

	int PULSES_PAR_TOUR = 3200;
	float DIAMETRE_ROUE = 7.62;
	float DistanceEntreRoue=19.5;

	ENCODER_ReadReset(1);
	ENCODER_Reset(0); // Encodeur gauche


	// Calculer la distance à parcourir pour tourner de 180 degrés
	float circonferenceRoue = PI * DIAMETRE_ROUE;
	float DistanceARouler = DistanceEntreRoue*2*PI*0.0625;

		while(ENCODER_Read(0)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)+80){
				MOTOR_SetSpeed(0 , -0.2);
			}

		MOTOR_SetSpeed(0 , 0);
		delay(100);

		while(ENCODER_Read(1)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR)-70){
			MOTOR_SetSpeed(1 , 0.2);
		}

		MOTOR_SetSpeed(1 , 0);
		delay(500);

	}

	if(direction==RIGHT){

		int PULSES_PAR_TOUR = 3200;
		float DIAMETRE_ROUE = 7.62;
		float DistanceEntreRoue=19.5;

		ENCODER_ReadReset(1);
		ENCODER_Reset(0); // Encodeur gauche


		// Calculer la distance à parcourir pour tourner de 180 degrés
		float circonferenceRoue = PI * DIAMETRE_ROUE;
		float DistanceARouler=DistanceEntreRoue*2.0*PI*0.0625;

		while(ENCODER_Read(0)<(DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR-80))
		{
			MOTOR_SetSpeed(0 , 0.2);

			Serial.print("Encodeur gauche \t");
			Serial.println(ENCODER_Read(0));

		}
		MOTOR_SetSpeed(0 , 0);
		delay(100);

		while(ENCODER_Read(1)>(-DistanceARouler/circonferenceRoue*PULSES_PAR_TOUR+70))
		{
			MOTOR_SetSpeed(1 , -0.2);

			Serial.print("Encodeur droit \t");
			Serial.println(ENCODER_Read(1));

		}
		MOTOR_SetSpeed(1 , 0);

	}
}

bool stoppingCriteria(){
	bool  breakState = 0;
	Serial.print("state->posCounter =");
	Serial.println(state->posCounter);
	
	switch(state->posCounter){

		case 0:
			updateDetectLine();
			state->posCounter += 1;
			breakState = 1;
			break;
		

		case 2:
			if(state->lapsCounter == 2){
				if(state->lookForWall){
					// detecteur infrarouge droite
				}
				else{
					// detecteur infrarouge avant
				}
				breakState = 1;
			}

			// detecteur de couleur
			else {
				getColorData();
				if(color.floorColor != color.CARPET){
					state->posCounter += 1;
					breakState = 1;

				}
				
			}
			break;

		case 4:
			if(/*Detecter le mur*/1){
				state->posCounter = 10;
				breakState = 1;

			}
			break;
		

		case 5:

			if(state->lapsCounter == 1 && state->cupIsDroped == 0){
				detecteurProximite();
				if(color.startColor == color.YELLOW){
					if(state->detectRight == 1){

						delay(1000); // À modifier
						
						stopMotors();

						turn(RIGHT);
						turn(RIGHT);


						state->posCounter = 7;
						breakState = 1;
					}
				}
				
				if(state->detectLeft == 1){
					dropTheCup();
					state->cupIsDroped = 1;
					delay(2000);
					dropTheCup();
				}
			}
			
			getColorData();
			
			Serial.println("l.234");
			if(color.floorColor != color.startColor || color.floorColor == color.WHITE){
				state->posCounter = 6;
				Serial.println("Stopping criteria white detected");
				breakState = 1;
			}
			break;
		
		case 6:

			updateDetectLine();
			if(state->lineDetectM == 0|| state->lineDetectL == 0 || state->lineDetectR == 0){
				Serial.println("detectLine");
				
				if(color.startColor == color.YELLOW && state->detectTwice == 0){
					delay(100);
					turnAngle(RIGHT,70);
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
					delay(1000);
					Serial.println("detectTwice == 0)");
					state->detectTwice = 1;
					break;
				}

				else if(color.startColor == color.YELLOW && state->detectTwice == 1){
					if((state->lineDetectL == 0 || state->lineDetectM == 0) && state->lineDetectR == 1){
						breakState = 1;
						Serial.println("detectTwice == 1)");
						break;
					}

					/*else if(state->lineDetectL == 0 && state->lineDetectM == 0 && state->lineDetectR == 1){
						breakState = 1;
						Serial.println("detectTwice == 1)");
						break;
					}*/
				}
				Serial.println("ligne 360");
				breakState = 1;
			}
			
			break;

		case 7:
			detecteurProximite();
			if(state->detectLeft == 1){
				dropTheCup();
				delay(2000); // À modifier
				stopMotors();
				state->cupIsDroped = 1;
				dropTheCup();
				turn(RIGHT);
				turn(RIGHT);
				
				state->posCounter = 5;
				breakState = 1;
			}
			break;

		case 8: // test detect couleur follow line
			getColorData();
			break;

		case 9: // test followline
			updateDetectLine();

			if(state->lineDetectL == 0 && state->lineDetectM == 0 && state->lineDetectR == 0 ){
				breakState = 1;
			}
			

			


	}

	return breakState;
}

void calibrateColor()
{
		
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

void followLine(){

	int biggerTHIRTY = 0;
	while(1){
		/*
		if(count == 10){
			float cm = convertDistanceIR(1);
			Serial.println(cm);
			count = 0;

		}
		delay(50);
		*/
	
		/*
		getColorData();
		//if(cm > 17.5)
		float cm = convertDistanceIR(1);
		Serial.println(cm);
		if(cm < 16){
			biggerTHIRTY = 1;
		}
		if(cm > 30 && biggerTHIRTY == 1){

			if(color.floorColor == color.YELLOW || color.floorColor == color.GREEN){
				Serial.print("color.startcolor = ");
				Serial.println(color.startColor);
				Serial.print("color.floorColor = ");
				Serial.println(color.floorColor);
				
				MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);
				MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);

				delay(250);

				
				break;
			}
		}
		

		updateDetectLine();
		Serial.print(" Detect = ");
		Serial.print(state->lineDetectL);
		Serial.print(" ");
		Serial.print(state->lineDetectM);
		Serial.print(" ");
		Serial.print(state->lineDetectR);
		Serial.println(" ");
		*/

		while(1){
			if(state->posCounter == 9){
				bool AdvanceGAUCHE = 0;
				bool AdvanceDROIT = 0;
				if( state->lineDetectL == 1 && state->lineDetectM == 0 && state->lineDetectR == 0){
					AdvanceDROIT = 1;
				}

				else if(state->lineDetectL == 1 && state->lineDetectM == 1 && state->lineDetectR == 0){
					AdvanceDROIT = 1;
				}

				else if(state->lineDetectL == 0 && state->lineDetectM == 1 && state->lineDetectR == 1){
					AdvanceGAUCHE = 1;
				}

				else if(state->lineDetectL == 0 && state->lineDetectM == 0 && state->lineDetectR == 1){
					AdvanceGAUCHE = 1;
				}

				else if(state->lineDetectL == 0 && state->lineDetectM == 0 && state -> lineDetectL == 0){
					return;
				}

				if(AdvanceDROIT == 1){
					while(state->lineDetectL != 0 && state->lineDetectM != 0 && state -> lineDetectL != 0){
						MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0.15);
						MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0);
					}
					return;
				}

				if(AdvanceGAUCHE == 1){
					while(state->lineDetectL != 0 && state->lineDetectM != 0 && state -> lineDetectL != 0){
						MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0);
						MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0.15);
					}
					return;
				}

				if(state->posCounter == 9){
					return;
				}

				if(state->posCounter == 0){
					// Gros turn
					return;
				}
			}

			else{
				if( state->lineDetectL == 1 && state->lineDetectM == 0 && state->lineDetectR == 0){
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0.15);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0);
					Serial.println("Droit");
				}

				else if(state->lineDetectL == 1 && state->lineDetectM == 1 && state->lineDetectR == 0){
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0.15);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0);
					Serial.println("Droit");
				}

				else if(state->lineDetectL == 0 && state->lineDetectM == 1 && state->lineDetectR == 1){
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0.15);
					Serial.println("Gauche");
				}

				else if(state->lineDetectL == 0 && state->lineDetectM == 0 && state->lineDetectR == 1){
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0.15);
					Serial.println("Gauche");
				}

				else if(state->lineDetectL == 1 && state->lineDetectM == 0 && state -> lineDetectL == 1){
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0.15);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0.15);
					Serial.println("Milieu");
				}

				else{
					MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0.15);
					MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0);
					Serial.println("Droit");
				}
			}
		}

	
		
	}

	state->posCounter = 9;
}

void forward(){
	state->moving = 1;
	int success = 0;
	// accélération
	while(stoppingCriteria() == 0){ //***CONDITION D'ARRET***

		if(*baseSet.affichage == 'Y'){
			Serial.print("success = ");
			Serial.println(success);
		}

		

		ENCODER_Reset(1);
		ENCODER_Reset(0);
	
		MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
		MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);

		for(int i = 0 ; i <= ((success+1)*5) ; i++){ // ici c'est pour mettre un delay(250) en s'assurant qu'il vérifie quand meme detecteurProx

			if(stoppingCriteria() == 1){ //***CONDITION D'ARRET***
				return;
			}
			delay(50);
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


	}
}

/*void largeTurn(){

	// Tourner

	if((state->lapsCounter == 1 || state->lapsCounter == 3) && state->posCounter == 3){
		state->posCounter += 2;
		return;
	}

	state->posCounter += 1;

}*/

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


  etat->begin = 1;

  etat->lapsCounter = 1;
  etat->posCounter = 0;

  etat->lookForWall = 0;

  etat->cupIsDroped = 0;

  etat->lineDetectL = 0;

  etat->lineDetectM = 0;

  etat->lineDetectR = 0;

  etat->detectTwice = 0;
  etat->yellowCnt = 0;

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
		
		if(success == 5){
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

		if(success == 6){
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


void largeTurn()
{
	int rightPulse = 0;
	int leftPulse = 0;

	int totalRight = 0;
	int totalLeft = 0;

	for (int section = 0; section < NB_SECTIONS; section++)
	{

		Serial << "Currently at section " << section << '\n';

		ENCODER_Reset(RIGHT);
		ENCODER_Reset(LEFT);

		rightPulse = 0;
		leftPulse = 0;

		while (rightPulse < (MAX_RIGHTlarge / NB_SECTIONS) && leftPulse < (MAX_LEFTlarge / NB_SECTIONS))
		{
			MOTOR_SetSpeed(RIGHT, 0.32);
			MOTOR_SetSpeed(LEFT, 0.4);

			rightPulse = ENCODER_Read(RIGHT);
			leftPulse = ENCODER_Read(LEFT);

			totalRight += rightPulse;
			totalLeft += leftPulse;
		}

		Serial << "Finished session " << section << '\n';
	}

	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);
}

void shortTurn()
{
	int rightPulse = 0;
	int leftPulse = 0;

	int totalRight = 0;
	int totalLeft = 0;

	for (int section = 0; section < NB_SECTIONS; section++)
	{

		Serial << "Currently at section " << section << '\n';

		ENCODER_Reset(RIGHT);
		ENCODER_Reset(LEFT);

		rightPulse = 0;
		leftPulse = 0;

		while (rightPulse < (MAX_RIGHTshort / NB_SECTIONS) && leftPulse < (MAX_LEFTshort / NB_SECTIONS))
		{
			MOTOR_SetSpeed(RIGHT, 0.28);
			MOTOR_SetSpeed(LEFT, 0.4);

			rightPulse = ENCODER_Read(RIGHT);
			leftPulse = ENCODER_Read(LEFT);

			totalRight += rightPulse;
			totalLeft += leftPulse;
		}

		Serial << "Finished session " << section << '\n';
	}

	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);
}


void decelatationCalibration(){
	int success = 0;



	while(1){
		if(*baseSet.affichage == 'Y'){
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
	while(ROBUS_IsBumper(LEFT)==0){
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

		if(ROBUS_IsBumper(LEFT)){
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

void dropTheCup(){

	if(state->cupIsDroped == 0){
		SERVO_SetAngle(1,180);
		
	}
    
    if(state->cupIsDroped == 1){
		SERVO_SetAngle(1,0);
	}
    
}

void loop() {
	
	if(!TEST){
		if(state->begin == 1){
			state->begin = 0;
			Serial.println("Voulez-vous afficher les valeurs des pulses et des vitesses?");
			Serial.println("| Yes : front bumper | No : rear bumper |");
			

			while(1){

				if(ROBUS_IsBumper(LEFT)){
					*baseSet.affichage = 'Y';
					while(1){if(!ROBUS_IsBumper(LEFT)){Serial.println("Yes");break;}delay(50);} // pour qu'il break qund le bumper est relaché
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

			if(detectFrequency()){
				state->moving = 1;
			}

			if(state->moving == 1){
				switch(state->posCounter){

					case 0:
						forward();
						break;
					
					case 1:
						largeTurn();
						break;

					case 2:
						if(state->lapsCounter == 2){
							motorsAccelerate();
							turn(RIGHT);
							forward();
							state->lookForWall = 1;
							turn(LEFT);
							forward();
							turn(RIGHT);
							state->posCounter += 2;
						}
						forward();
						break;

					case 3:
						largeTurn();
						break;

					case 4:
						forward(); 
						break;

					case 5:
						forward();
						break;

					case 6:
						forward();
						followLine();
						state->posCounter = 9;
						break;

					case 7:
						forward();
						break;

					case 8:
						
						break;

					case 9:
						
						forward();
						state->posCounter = 0;
						state->lapsCounter++;
						break;

					case 10:
						
						state->posCounter = 9;
						break;

					
				}
			}
		}

		if(CALIBRATEMOTORS == 1){
			motorCalibration();
			CALIBRATEMOTORS = 0;
		}
	}

	if(TEST){


		if(TEST_followTheLine){
			while(1){
				if(ROBUS_IsBumper(LEFT)==1){
					break;
				}
			}
			
			/*float cm = convertDistanceIR(1);
			Serial.println(cm);*/
			state->lapsCounter = 1;
			state->posCounter = 5;

			forward();
			forward();
	
			followLine();

			forward();

		}

		if(TEST_detectColor){
			while(1){
				if(ROBUS_IsBumper(LEFT)==1){
					break;
				}
			}
			Serial.println("TEST_detectcolor");
			getColorData();
			


		}

		if(TEST_turn){

			while(1){
				if(ROBUS_IsBumper(LEFT)==1){
					break;
				}
			}

			turn45(RIGHT);
			delay(1000);

		}

		if(HARDCODE){
			while(1){
				if(ROBUS_IsBumper(LEFT)==1){
					break;
				}
			}
			state->posCounter = 9;
			forward();
			if(color.startColor == color.GREEN)
				shortTurn();
			else
				largeTurn();
			state->posCounter = 2;
			forward();
			if(color.startColor == color.GREEN)
				shortTurn();
			else
				largeTurn();
			state->posCounter = 5;
			forward();
			if(color.startColor == color.GREEN)
				shortTurn();
			else
				largeTurn();
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
			delay(1000);
			if(color.startColor == color.GREEN)
				shortTurn();
			else
				largeTurn();
			turnAngle(RIGHT,90);
			getColorData();
			while(color.floorColor != color.BLUE){
				getColorData();
				MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);
				MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
			}
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, speed->forwardLeft);
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, speed->forwardRight);
			delay(100);
			MOTOR_SetSpeed(baseSet.MOTOR_LEFT, 0);
			MOTOR_SetSpeed(baseSet.MOTOR_RIGHT, 0);
			turnAngle(LEFT,90);

		}

	}

	

}