
#include <Arduino.h>
#include <LibRobus.h>


#define PIN_PROX_RIGHT 44
#define PIN_PROX_LEFT 42

#define PIN_SND_AMB PINA0
#define PIN_SND_FRQ PINA1

#define MAX_SPEED 0.5
#define TURNING_SPEED 0.2

#define LEFTX 0
#define MIDDLEX 1
#define RIGHTX 2

#define FOWARD 2

const int UNIT_SIZE = 4000;

// #define ENCODER_LEFT 0
// #define ENCODER_RIGHT 1

// #define MOTOR_LEFT 0
// #define MOTOR_RIGHT 1


// Functions related to the sound sensor

int getRightProx() { return digitalRead(PIN_PROX_RIGHT); }
int getLeftProx() { return digitalRead(PIN_PROX_LEFT); }


// Function related to the proximity sensor

float getAmbient() { return analogRead(PIN_SND_AMB); }
float getFrequency() { return analogRead(PIN_SND_FRQ); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }

void stop(){MOTOR_SetSpeed(RIGHT, 0);MOTOR_SetSpeed(LEFT, 0);delay(100);}
void turnRight(){
	int leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	rightSpeed = -TURNING_SPEED;
	leftSpeed = TURNING_SPEED;
	// Moving right wheel
	while (rightDistance < 1900) {
		MOTOR_SetSpeed(RIGHT, rightSpeed);
		rightDistance = -ENCODER_Read(RIGHT);
		leftDistance = ENCODER_Read(LEFT);
		delay(20);
	}
	// Making sure the robot stops
	MOTOR_SetSpeed(RIGHT, 0);
	delay(100);
	// Moving left wheel
	while (leftDistance < 1900) {
		MOTOR_SetSpeed(LEFT, leftSpeed);
		rightDistance = -ENCODER_Read(RIGHT);
		leftDistance = ENCODER_Read(LEFT);
		delay(20);
	}
	// Making sure the robot stops
	MOTOR_SetSpeed(LEFT, 0);
	delay(100);
}
void turnLeft(){
	int leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	rightSpeed = TURNING_SPEED;
	leftSpeed = -TURNING_SPEED;
	// Moving left wheel
	while (leftDistance < 1900) {
		MOTOR_SetSpeed(LEFT, leftSpeed);
		rightDistance = ENCODER_Read(RIGHT);
		leftDistance = -ENCODER_Read(LEFT);
		delay(20);
	}
	// Making sure the robot stops
	MOTOR_SetSpeed(LEFT, 0);
	delay(100);
	// Moving right wheel
	while (rightDistance < 1900) {
		MOTOR_SetSpeed(RIGHT, rightSpeed);
		rightDistance = ENCODER_Read(RIGHT);
		leftDistance = -ENCODER_Read(LEFT);
		delay(20);

	}
	// Making sure the robot stops
	MOTOR_SetSpeed(RIGHT, 0);
	delay(100);
}

void oneEighty(bool direction){
	if(direction == LEFT){
		for(uint8_t i = 0; i < 2; i++){
			turnLeft();
		}
	}
	else if(direction == RIGHT){
		for(uint8_t i = 0; i < 2; i++){
			turnRight();
		}
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
	for (int i = 0; i < 10; i += 2) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);
		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.01; leftSpeed -= 0.01; }
		else { rightSpeed -= 0.01; leftSpeed += 0.01; }
		delay(100);
	}
	// Updating distance traveled
	mainDistance += (ENCODER_ReadReset(RIGHT) + ENCODER_ReadReset(LEFT)) / 2;
	// Moving forward and self correcting
	while (mainDistance < UNIT_SIZE) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.005; leftSpeed -= 0.005; }
		else { rightSpeed -= 0.005; leftSpeed += 0.005; }

		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);
		
		mainDistance += (rightDistance + leftDistance) / 2;

		delay(50);

	}
	// Deccelerate
	for (int i = 10; i > 0; i -= 2) {
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);

		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.01; leftSpeed -= 0.01; }
		else { rightSpeed -= 0.01; leftSpeed += 0.01; }
		delay(100);
	}
	// Making sure the robot stops
	stop();
}
bool go = false;
void setup() {
	BoardInit();
	delay(2000);
}
uint8_t posY = 0;//default
uint8_t posX = MIDDLEX;//default
uint8_t lastPosX = MIDDLEX;//default
uint8_t dir = FOWARD;//default
void loop() {
	delay(100);
	if(detectFrequency()){Serial.println(detectFrequency());go = true;}
	if(go == true){
		if(getRightProx()==1&&getLeftProx()==1){
			switch (dir)
			{
				case FOWARD:
					advanceUnit();
					posY++;
					if(posY == 10){go = false;stop();}
					break;
				case LEFT:
					if(posX != LEFTX){
						advanceUnit();
						posX--;
					}
					turnRight();
					dir=FOWARD;
					break;
				case RIGHT:
					if(posX != RIGHTX){
						advanceUnit();
						posX++;
					}
					turnLeft();
					dir=FOWARD;
					break;
			}
			Serial.println(posX);
		}
		else{
			stop();
			delay(100);
			switch (dir)
			{
			case FOWARD:
				if(posX == MIDDLEX){
					turnLeft();
					dir = LEFT;
					if(getRightProx()==0||getLeftProx()==0){
						oneEighty(RIGHT);
						dir = RIGHT;
					}
				}
				else if(posX == LEFTX){
					turnRight();
					dir = RIGHT;
				}
				else if(posX == RIGHTX){
					turnLeft();
					dir = LEFT;
				}
				break;
			case LEFT:
				turnRight();
				dir = FOWARD;
				break;
			case RIGHT:
				turnLeft();
				dir = FOWARD;
				break;
			}
		}
	}	
}