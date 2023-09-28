#include <Arduino.h>
#include <LibRobus.h>

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

#define capDroite 44
#define led_capDroite 45

#define capGauche 42
#define led_capGauche 43

void foward(float _speed);
void stopMotors();
void motorsAccelerate(float _speedWanted);

struct Pulse {
  uint16_t right;
  uint16_t left;
  uint16_t rightCnt = 0;
  uint16_t leftCnt = 0;
  uint16_t rightPast;
  uint16_t leftPast;
};

struct Pulse pulse;
float speed;
void setup() {
  BoardInit();
//   Serial.begin(9600);
  speed=1;
 

  //detecteur de proximité
  pinMode(42, INPUT); //capDroite
  pinMode(44, INPUT); //capGauche
  pinMode(43, OUTPUT); //ledcapDroite
  pinMode(45, OUTPUT); //ledcapGauche
  
  
}


void motorsAccelerate(float _speedWanted){
  uint8_t delayMs = 150;
  foward(_speedWanted*0.10);
  delay(delayMs);
  foward(_speedWanted*0.20);
  delay(delayMs);
  foward(_speedWanted*0.30);
  delay(delayMs);
  foward(_speedWanted*0.40);
  delay(delayMs);
  foward(_speedWanted*0.50);
  delay(delayMs);
  foward(_speedWanted*0.60);
  delay(delayMs);
  foward(_speedWanted*0.70);
  delay(delayMs);
  foward(_speedWanted*0.80);
  delay(delayMs);
  foward(_speedWanted*0.90);
  delay(delayMs);
  foward(_speedWanted);
}
void foward(float _speed){
  if(pulse.right < pulse.left) {
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed+0.15);
    MOTOR_SetSpeed(MOTOR_LEFT,_speed-0.15);
  }
  if(pulse.right > pulse.left) {
    MOTOR_SetSpeed(MOTOR_LEFT,_speed+0.15);
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed-0.15);
  }
  if(pulse.right == pulse.left) {
    MOTOR_SetSpeed(MOTOR_LEFT,_speed);
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed);
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
 MOTOR_SetSpeed(MOTOR_LEFT,0);
 MOTOR_SetSpeed(MOTOR_RIGHT,0);
}

void detecteurProximite(){


	if(digitalRead(capDroite)==HIGH){
    	digitalWrite(led_capDroite, HIGH);
    } else {
		digitalWrite(led_capDroite, LOW);
	}

	if(digitalRead(capGauche)==HIGH){
    	digitalWrite(led_capGauche, HIGH);
    } else {
		digitalWrite(led_capGauche, LOW);
	}

    delay(100);
}

void loop() {

    detecteurProximite();
   
}