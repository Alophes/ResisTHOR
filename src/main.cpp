
#include <Arduino.h>
#include <LibRobus.h>


#define PIN_PROX_RIGHT 44
#define PIN_PROX_LEFT 42

#define PIN_SND_AMB PINA0
#define PIN_SND_FRQ PINA1

#define MAX_SPEED 0.8
#define CYCLE 2040

const int UNIT_SIZE = 10000;

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


// Function related to the movement of the robot

void advanceUnit() {

	int mainDistance = 0, leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;

	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);

	rightSpeed = MAX_SPEED;
	leftSpeed = MAX_SPEED; 

	// Accelerate
	for (int i = 0; i < 10; i += 2) {

		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);

		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.005; leftSpeed -= 0.005; }
		else { rightDistance -= 0.005; leftSpeed += 0.005; }

		delay(200);

	}
	
	// Updating distance traveled
	mainDistance += (ENCODER_ReadReset(RIGHT) + ENCODER_ReadReset(LEFT)) / 2;

	// Moving forward and self correcting
	while (mainDistance < UNIT_SIZE) {

		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);

		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.005; leftSpeed -= 0.005; }
		else { rightDistance -= 0.005; leftSpeed += 0.005; }

		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);
		
		mainDistance += (rightDistance + leftDistance) / 2;

		delay(250);

	}

	// Deccelerate
	for (int i = 10; i > 0; i -= 2) {

		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);

		// Self correcting
		if (rightDistance < leftDistance) { rightSpeed += 0.005; leftSpeed -= 0.005; }
		else { rightDistance -= 0.005; leftSpeed += 0.005; }

		delay(200);

	}

	// Making sure the robot stops
	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);

}


void setup() {

	BoardInit();

}



void loop() {

	Serial << "START";
	advanceUnit();

	Serial << "PAUSE";
	delay(2000);

}




// void foward(float _speed);
// void stopMotors();
// void motorsAccelerate(float _speedWanted);

// struct Pulse {
//   uint16_t right;
//   uint16_t left;
//   uint16_t rightCnt = 0;
//   uint16_t leftCnt = 0;
//   uint16_t rightPast;
//   uint16_t leftPast;
// };

// struct Pulse pulse;
// float speed;
// void setup() {
//   BoardInit();
//   Serial.begin(9600);
//   speed=1;
//   motorsAccelerate(speed);
// }

// void loop() {
//   //Serial.println(ENCODER_Read(1));
//   pulse.leftPast=pulse.left;
//   pulse.rightPast=pulse.right;
//   pulse.right = ENCODER_Read(ENCODER_RIGHT);
//   pulse.left = ENCODER_Read(ENCODER_LEFT);
//   if (pulse.left >= 3200) {
//     ENCODER_Reset(ENCODER_LEFT);
//     pulse.leftCnt++;
//   }
//   if (pulse.right >= 3200) {
//     ENCODER_Reset(ENCODER_RIGHT);
//     pulse.rightCnt++;
//   }
//   if(pulse.rightCnt >= 53 && pulse.leftCnt >= 53)
//     stopMotors();
//   else
//     foward(speed);

// }

// void motorsAccelerate(float _speedWanted){
//   uint8_t delayMs = 150;
//   foward(_speedWanted*0.10);
//   delay(delayMs);
//   foward(_speedWanted*0.20);
//   delay(delayMs);
//   foward(_speedWanted*0.30);
//   delay(delayMs);
//   foward(_speedWanted*0.40);
//   delay(delayMs);
//   foward(_speedWanted*0.50);
//   delay(delayMs);
//   foward(_speedWanted*0.60);
//   delay(delayMs);
//   foward(_speedWanted*0.70);
//   delay(delayMs);
//   foward(_speedWanted*0.80);
//   delay(delayMs);
//   foward(_speedWanted*0.90);
//   delay(delayMs);
//   foward(_speedWanted);
// }

// void foward(float _speed){
//   if(pulse.right < pulse.left) {
//     MOTOR_SetSpeed(MOTOR_RIGHT,_speed+0.15);
//     MOTOR_SetSpeed(MOTOR_LEFT,_speed-0.15);
//   }
//   if(pulse.right > pulse.left) {
//     MOTOR_SetSpeed(MOTOR_LEFT,_speed+0.15);
//     MOTOR_SetSpeed(MOTOR_RIGHT,_speed-0.15);
//   }
//   if(pulse.right == pulse.left) {
//     MOTOR_SetSpeed(MOTOR_LEFT,_speed);
//     MOTOR_SetSpeed(MOTOR_RIGHT,_speed);
//   }
//   if(pulse.leftPast != pulse.left) {
//   Serial.print("LEFT ");
//   Serial.println(pulse.left);
//   }
//   if(pulse.rightPast != pulse.right) {
//   Serial.print("RIGHT ");
//   Serial.println(pulse.right);
//   }
// }


// void stopMotors(){
//  MOTOR_SetSpeed(MOTOR_LEFT,0);
//  MOTOR_SetSpeed(MOTOR_RIGHT,0);
// }