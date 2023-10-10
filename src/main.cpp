
#include <Arduino.h>
#include <LibRobus.h>


#define PIN_PROX_RIGHT 44
#define PIN_PROX_LEFT 42

#define PIN_SND_AMB PINA0
#define PIN_SND_FRQ PINA1

#define MAX_SPEED 0.5
#define TURNING_SPEED 0.2
#define MAX_TURNING_DISTANCE 1870
#define START_POSITION 1943
#define KP 0.005
#define KP_ACCELERATION 0.0025
#define KP_DECELERATION 0.0025

//x axis positions
#define LEFTX 0
#define MIDDLEX 1
#define RIGHTX 2


const uint16_t UNIT_SIZE = 3900;

// Functions related to the sound sensor
int getRightProx() { return digitalRead(PIN_PROX_RIGHT); }
int getLeftProx() { return digitalRead(PIN_PROX_LEFT); }

// Function related to the proximity sensor                       pourquoi des float et pas des int?
float getAmbient() { return analogRead(PIN_SND_AMB); }
float getFrequency() { return analogRead(PIN_SND_FRQ); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }

void stop(){MOTOR_SetSpeed(RIGHT, 0);MOTOR_SetSpeed(LEFT, 0);delay(100);}

void getInPosition(){
	uint16_t leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	rightSpeed = TURNING_SPEED; 
	leftSpeed = TURNING_SPEED;
	while (rightDistance < START_POSITION && leftDistance < START_POSITION) {
		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);
		rightDistance = ENCODER_Read(RIGHT);
		leftDistance = ENCODER_Read(LEFT);
		delay(20);
	}
	stop();
	delay(100);
}
void turn(bool dir){
	int leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	if(dir == RIGHT){
		rightSpeed = -TURNING_SPEED; 
		leftSpeed = TURNING_SPEED;
		// Moving right wheel
		while (rightDistance < MAX_TURNING_DISTANCE) {
			MOTOR_SetSpeed(RIGHT, rightSpeed);
			rightDistance = -ENCODER_Read(RIGHT);
			leftDistance = ENCODER_Read(LEFT);
			delay(20);
		}
		// Making sure the robot stops
		MOTOR_SetSpeed(RIGHT, 0);
		delay(100);
		// Moving left wheel
		while (leftDistance < MAX_TURNING_DISTANCE) {
			MOTOR_SetSpeed(LEFT, leftSpeed);
			rightDistance = -ENCODER_Read(RIGHT);
			leftDistance = ENCODER_Read(LEFT);
			delay(20);
		}
		// Making sure the robot stops
		MOTOR_SetSpeed(LEFT, 0);
	}
	else{
		rightSpeed = TURNING_SPEED;
		leftSpeed = -TURNING_SPEED;
		// Moving left wheel
		while (leftDistance < MAX_TURNING_DISTANCE) {
			MOTOR_SetSpeed(LEFT, leftSpeed);
			rightDistance = ENCODER_Read(RIGHT);
			leftDistance = -ENCODER_Read(LEFT);
			delay(20);
		}
		// Making sure the robot stops
		MOTOR_SetSpeed(LEFT, 0);
		delay(100);
		// Moving right wheel
		while (rightDistance < MAX_TURNING_DISTANCE) {
			MOTOR_SetSpeed(RIGHT, rightSpeed);
			rightDistance = ENCODER_Read(RIGHT);
			leftDistance = -ENCODER_Read(LEFT);
			delay(20);

		}
		// Making sure the robot stops
		MOTOR_SetSpeed(RIGHT, 0);
	}
	delay(100);
}
void oneEighty(bool direction){
	for(uint8_t i = 0; i < 2; i++){
		turn(direction);
	}
}
void advanceUnit() {
	int mainDistance = 0, leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	rightSpeed = MAX_SPEED;
	leftSpeed = MAX_SPEED; 
	// Accelerate
	for (uint8_t i = 0; i < 10; i += 2) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);
		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += KP_ACCELERATION; leftSpeed -= KP_ACCELERATION; }
		else { rightSpeed -= KP_ACCELERATION; leftSpeed += KP_ACCELERATION; }
		delay(100);
	}
	// Updating distance traveled
	mainDistance += (ENCODER_ReadReset(RIGHT) + ENCODER_ReadReset(LEFT)) / 2;
	// Moving forward and self correcting
	while (mainDistance < UNIT_SIZE) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += KP; leftSpeed -= KP; }
		else { rightSpeed -= KP; leftSpeed += KP; }

		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);
		
		mainDistance += (rightDistance + leftDistance) / 2;

		delay(50);
		
		if(getRightProx()==0||getLeftProx()==0)
			break;

	}
	// Deccelerate
	for (uint8_t i = 10; i > 0; i -= 2) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);

		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += KP_DECELERATION; leftSpeed -= KP_DECELERATION; }
		else { rightSpeed -= KP_DECELERATION; leftSpeed += KP_DECELERATION; }
		delay(100);
	}
	// Making sure the robot stops
	stop();
}
bool go = false;
void setup() {
	BoardInit();
}
uint8_t posY = 0;//default
uint8_t posX = MIDDLEX;//default

void loop() {
	//delay(1000);
	//turn(RIGHT);
	/*if(getRightProx()==1||getLeftProx()==1){advanceUnit();}
	delay(50);*/

	if(detectFrequency()){Serial.println(detectFrequency());go = true; getInPosition();}
	if(go == true){
		if(getRightProx()==0||getLeftProx()==0){
			if(posY<9){
				switch (posX)
				{
					case MIDDLEX:
						turn(LEFT);
						if(getRightProx()==0||getLeftProx()==0){
							oneEighty(RIGHT);
							advanceUnit();
							turn(LEFT);
							posX = RIGHTX;
						}
						else{
							advanceUnit();
							turn(RIGHT);
							posX = LEFTX;
						}
						if(getRightProx()==0||getLeftProx()==0){
							turn(RIGHT);
							for(uint8_t i = 0; i < 2; i++){advanceUnit();}
							turn(LEFT);
							posX = RIGHTX;
						}
						break;
					case LEFTX:
						turn(RIGHT);
						advanceUnit();
						turn(LEFT);
						posX = MIDDLEX;
						if(getRightProx()==0||getLeftProx()==0){
							turn(RIGHT);
							if(getRightProx()==0||getLeftProx()==0){
								turn(RIGHT);
								for(uint8_t i = 0; i < 2; i++){advanceUnit();}
								posY-=2;
								turn(LEFT);
								advanceUnit();
								turn(LEFT);
								posX = RIGHTX;
							}
							else{
								advanceUnit();
								turn(LEFT);
								posX = RIGHTX;
							}
						}
						break;
					case RIGHTX:
						turn(LEFT);
						advanceUnit();
						turn(RIGHT);
						posX = MIDDLEX;
						if(getRightProx()==0||getLeftProx()==0){
							turn(LEFT);
							if(getRightProx()==0||getLeftProx()==0){
								turn(LEFT);
								for(uint8_t i = 0; i < 2; i++){advanceUnit();}
								posY-=2;
								turn(RIGHT);
								advanceUnit();
								turn(RIGHT);
								posX = LEFTX;
							}
							else{
								advanceUnit();
								turn(RIGHT);
								posX = LEFTX;
							}
						}
						break;
				}
			}
		}
		else{
			if(posY < 9){advanceUnit(); posY++;}
			else{go = false; stop(); posY = 0; posX = MIDDLEX;}
		}
	}	
	delay(100);
}