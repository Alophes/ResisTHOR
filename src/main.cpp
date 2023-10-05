#include <Arduino.h>
#include <LibRobus.h>
#include "util.h"


void foward(float acceleration);
void stopMotors();
void motorsAccelerate();
Speed *initSpeed();
State *initState();

//detecteur de sifflet
float getAmbient() { return analogRead(PINA0); }
float getFrequency() { return analogRead(PINA1); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }




Pulse pulse;
BasicSettings BaseSet;
Pin pin;

//initialisation des variable de base
Speed *speed = initSpeed();
State *state = initState();


void setup() {
  BoardInit();

  
//   Serial.begin(9600);

 

  //detecteur de proximité
  pinMode(pin.capGauche, INPUT); //Pin.capDroite
  pinMode(pin.capGauche, INPUT); //capGauche
  pinMode(pin.led_capDroite, OUTPUT); //ledPin.capDroite
  pinMode(pin.led_capGauche, OUTPUT); //ledcapGauche
  
  //detecteur de sifflet
  pinMode(PINA0, INPUT);
  pinMode(PINA1, INPUT);
  
}


void motorsAccelerate(){
  uint8_t delayMs = 150;
  foward(0.10);
  delay(delayMs);
  foward(0.20);
  delay(delayMs);
  foward(0.30);
  delay(delayMs);
  foward(0.40);
  delay(delayMs);
  foward(0.50);
  delay(delayMs);
  foward(0.60);
  delay(delayMs);
  foward(0.70);
  delay(delayMs);
  foward(0.80);
  delay(delayMs);
  foward(0.90);
  delay(delayMs);
  foward(1);
}
void foward(float acceleration){
  state->moving = 1;
  if(pulse.right < pulse.left) {
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,(speed->motorRight-((speed->motorRight-speed->motorLeft)*BaseSet.KP))*acceleration);
    MOTOR_SetSpeed(BaseSet.ENCODER_RIGHT,(speed->motorLeft+((speed->motorRight-speed->motorLeft)*BaseSet.KP))*acceleration);
  }
  if(pulse.right > pulse.left) {
    MOTOR_SetSpeed(BaseSet.ENCODER_RIGHT,(speed->motorLeft-((speed->motorLeft-speed->motorRight)*BaseSet.KP))*acceleration);
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,(speed->motorRight+((speed->motorLeft-speed->motorRight)*BaseSet.KP))*acceleration);
  }
  if(pulse.right == pulse.left) {
    MOTOR_SetSpeed(BaseSet.ENCODER_RIGHT,speed->motorLeft*acceleration);
    MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,speed->motorRight*acceleration);
  }
  if(pulse.leftPast != pulse.left) {
  Serial.print("LEFT ");
  Serial.println(pulse.left);
  }
  if(pulse.rightPast != pulse.right) {
  Serial.print("RIGHT ");
  Serial.println(pulse.right);
  }
}


void stopMotors(){
  state->moving = 0;
  MOTOR_SetSpeed(BaseSet.ENCODER_RIGHT,0);
  MOTOR_SetSpeed(BaseSet.MOTOR_RIGHT,0);
}

void detecteurProximite(){


  
	if(digitalRead(pin.capDroite)==HIGH){ //Détection à droite
    	digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
      state->detectRight = 1;
    } else {
		digitalWrite(pin.led_capDroite, LOW);
    state->detectRight = 0;
	}

  
	if(digitalRead(pin.capGauche)==HIGH){ //Détection à gauche
    	digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
      state->detectLeft = 1;
    } else {
		digitalWrite(pin.led_capGauche, LOW);
    state->detectLeft = 0;
	}

}

Speed *initSpeed(){

  Speed *vitesse = (Speed*)malloc(sizeof(Speed));

  vitesse->motorLeft = BaseSet.speed_ini;
  vitesse->motorRight = BaseSet.speed_ini;

  return vitesse;
}

State *initState(){

  State *etat = (State*)malloc(sizeof(State));

  etat->coordinates[0] = 0;
  etat->coordinates[1] = 0;

  etat->detectLeft = 0;
  etat->detectRight = 0;


  return etat;
}

void loop() {

  detecteurProximite();
  
  if(state->detectLeft == 1 || state->detectRight == 1){
    stopMotors();
  }
  else{
    motorsAccelerate();
    foward(1);
  }

   
}