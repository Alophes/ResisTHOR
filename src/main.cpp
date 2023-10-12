
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

// x axis positions

#define RIGHTX 0
#define MIDDLEX 1
#define LEFTX 2


/*
0 = forward
1 = left
2 = backwards
3 = right
*/
// Directions
#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3 

uint8_t posY = 0;		// default
uint8_t posX = MIDDLEX; // default

int horizontal_lines[9][3] = {
	{0, 1, 0}, 
	{0, 1, 0},
	{0, 1, 0},
	{0, 1, 0},
	{0, 1, 0},
	{0, 0, 0},
	{0, 1, 0},
	{0, 1, 0},
	{0, 1, 0}};


// 0 vert droite
// 1 vert gauche
int vertical_lines[10][2] = {
	{0, 0},
	{1, 1},
	{0, 0},
	{1, 1},
	{0, 0},
	{1, 1},
	{0, 1},
	{1, 1},
	{0, 0},
	{1, 1}};

uint8_t decelerationDelay = 100;

const uint16_t UNIT_SIZE = 3650;

// Functions related to the sound sensor
int getRightProx() { return digitalRead(PIN_PROX_RIGHT); }
int getLeftProx() { return digitalRead(PIN_PROX_LEFT); }

// Function related to the proximity sensor                       pourquoi des float et pas des int?
float getAmbient() { return analogRead(PIN_SND_AMB); }
float getFrequency() { return analogRead(PIN_SND_FRQ); }

int detectFrequency() { return (-(getAmbient() - 45) + getFrequency() > 50); }

void stop()
{

	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);
	delay(100);
}

void getInPosition()
{

	uint16_t leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;

	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);

	rightSpeed = TURNING_SPEED;
	leftSpeed = TURNING_SPEED;

	while (rightDistance < START_POSITION && leftDistance < START_POSITION)
	{

		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);

		rightDistance = ENCODER_Read(RIGHT);
		leftDistance = ENCODER_Read(LEFT);

		delay(20);
	}

	stop();
	delay(100);
}


/*
0 = forward
1 = left
2 = backwards
3 = right
*/
int direction = 0;

uint8_t movementTab[50];
uint8_t mCnt = 0;


void turn(bool dir)
{

	int leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;

	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);

	if (dir == RIGHT)
	{

		movementTab[mCnt++] = 2;

		if (direction == 0 ) { direction = 3; }
		else { direction--; }

		rightSpeed = -TURNING_SPEED;
		leftSpeed = TURNING_SPEED;

		// Moving right wheel
		while (rightDistance < MAX_TURNING_DISTANCE)
		{

			MOTOR_SetSpeed(RIGHT, rightSpeed);

			rightDistance = -ENCODER_Read(RIGHT);
			leftDistance = ENCODER_Read(LEFT);

			delay(20);
		}

		// Making sure the robot stops
		MOTOR_SetSpeed(RIGHT, 0);
		delay(100);

		// Moving left wheel
		while (leftDistance < MAX_TURNING_DISTANCE)
		{

			MOTOR_SetSpeed(LEFT, leftSpeed);

			rightDistance = -ENCODER_Read(RIGHT);
			leftDistance = ENCODER_Read(LEFT);

			delay(20);
		}

		// Making sure the robot stops
		MOTOR_SetSpeed(LEFT, 0);
	}

	else
	{

		movementTab[mCnt++] = 3;

		if (direction == 3 ) { direction = 0; }
		else { direction++; }

		rightSpeed = TURNING_SPEED;
		leftSpeed = -TURNING_SPEED;

		// Moving left wheel
		while (leftDistance < MAX_TURNING_DISTANCE)
		{

			MOTOR_SetSpeed(LEFT, leftSpeed);

			rightDistance = ENCODER_Read(RIGHT);
			leftDistance = -ENCODER_Read(LEFT);

			delay(20);
		}

		// Making sure the robot stops
		MOTOR_SetSpeed(LEFT, 0);
		delay(100);

		// Moving right wheel
		while (rightDistance < MAX_TURNING_DISTANCE)
		{

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

void oneEighty(bool direction)
{

	for (uint8_t i = 0; i < 2; i++)
	{

		turn(direction);
	}
}
void advanceUnit()
{
	int mainDistance = 0, leftDistance = 0, rightDistance = 0;
	float leftSpeed, rightSpeed;
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	rightSpeed = MAX_SPEED;
	leftSpeed = MAX_SPEED;
	// Accelerate
	for (uint8_t i = 0; i < 10; i += 2)
	{
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);
		// Self correcting
		if (rightDistance < leftDistance)
		{
			rightSpeed += KP_ACCELERATION;
			leftSpeed -= KP_ACCELERATION;
		}
		else
		{
			rightSpeed -= KP_ACCELERATION;
			leftSpeed += KP_ACCELERATION;
		}
		delay(100);
	}
	// Updating distance traveled
	mainDistance += (ENCODER_ReadReset(RIGHT) + ENCODER_ReadReset(LEFT)) / 2;
	// Moving forward and self correcting
	while (mainDistance < UNIT_SIZE)
	{
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		// Self correcting
		if (rightDistance < leftDistance)
		{
			rightSpeed += KP;
			leftSpeed -= KP;
		}
		else
		{
			rightSpeed -= KP;
			leftSpeed += KP;
		}

		MOTOR_SetSpeed(RIGHT, rightSpeed);
		MOTOR_SetSpeed(LEFT, leftSpeed);

		mainDistance += (rightDistance + leftDistance) / 2;

		delay(50);

		if (getRightProx() == 0 || getLeftProx() == 0)
		{
			decelerationDelay = 150;
			delay(50);
		}
		if (getRightProx() == 0 || getLeftProx() == 0)
			break;
	}
	// Deccelerate
	for (uint8_t i = 10; i > 0; i -= 2)
	{
		rightDistance = ENCODER_ReadReset(RIGHT);
		leftDistance = ENCODER_ReadReset(LEFT);
		MOTOR_SetSpeed(RIGHT, (i * 0.1) * rightSpeed);
		MOTOR_SetSpeed(LEFT, (i * 0.1) * leftSpeed);

		// Self correcting
		if (rightDistance < leftDistance)
		{
			rightSpeed += KP_DECELERATION;
			leftSpeed -= KP_DECELERATION;
		}
		else
		{
			rightSpeed -= KP_DECELERATION;
			leftSpeed += KP_DECELERATION;
		}
		delay(decelerationDelay);
	}
	decelerationDelay = 100;
	// Making sure the robot stops
	stop();

	movementTab[mCnt++] = 1;

	switch (direction)
	{
		case NORTH:
			posY++;
			break;
		
		case SOUTH:
			posY--;
			break;

		case EAST:
			posX--;
			break;
		
		case WEST:
			posX++;
			break;
	}

}

/*
0 = no movements
1 = foward
2 = turn right
3 = turn left
*/
bool go = false;
bool comeBack = false;

bool isDeadEnd = false;
uint8_t nbDeadMove = 0;

void resetMovementTab()
{
	for (uint8_t i = 0; i < 50; i++)
	{
		movementTab[i] = 0;
	}
}
void printMovementTab()
{
	for (uint8_t i = 0; i < mCnt; i++)
	{
		Serial.println(movementTab[i]);
	}
}
void goBack()
{
	Serial.print("mCnt = ");
	Serial.println(mCnt);
	printMovementTab();
	oneEighty(RIGHT);
	for (uint8_t i = 0; i < mCnt + 1; i++)
	{
		switch (movementTab[mCnt - i])
		{
		case 1: // foward
			advanceUnit();
			break;
		case 2: // right
			turn(LEFT);
			break;
		case 3: // left
			turn(RIGHT);
			break;
		case 0: // no movements
			stop();
			break;
		}
	}
	oneEighty(RIGHT);
	resetMovementTab();
	posY = 0;
	posX = MIDDLEX; // default
	comeBack = false;
}


void deadEnd() {



}

void setup()
{
	for (uint8_t i = 0; i < 50; i++)
	{
		movementTab[i] = 0;
	}
	BoardInit();
}

void loop()
{

	if (comeBack == false) {
		if (detectFrequency()) { go = true; getInPosition(); }
	}

	if (go) {

		if (getRightProx() == 0 || getLeftProx() == 0) {

			switch (direction) {

				case NORTH:
					horizontal_lines[posY][posX] = 1;
					break;
				
				case SOUTH:
					horizontal_lines[posY - 1][posX] = 1;
					break;

				case EAST:
					if (posX == LEFTX) { vertical_lines[posY][1] = 1; }
					else { vertical_lines[posY][0] = 1; }
					break;

				case WEST:
					if (posX == RIGHT) { vertical_lines[posY][0] = 1; }
					else { vertical_lines[posY][1] = 1; }
					break;

			}

		} 

		switch (direction) {


			case NORTH:

				if (horizontal_lines[posY][posX]) {				

					switch (posX) {

						case MIDDLEX:
							
							if (vertical_lines[posY][0] == 0) { turn(RIGHT); }
							else if (vertical_lines[posY][1] == 0) { turn(LEFT);} 
							else { oneEighty(RIGHT); }
							break;

						case RIGHTX: 

							if (vertical_lines[posY][0] == 0) { turn(LEFT); }
							else { oneEighty(RIGHT); }
							break;
						
						case LEFTX:

							if (vertical_lines[posY][1] == 0) { turn(RIGHT); }
							else { oneEighty(RIGHT); }
							break;

					}	
				} else { Serial << "front clear\n"; }

				break;
			
			case EAST:

				switch (posX) {

					case MIDDLEX:
						
						if (horizontal_lines[posY][posX] == 0) { turn(LEFT); }
						else if (vertical_lines[posY][0] == 1) { turn(RIGHT); }
						break;

					case RIGHTX: 

						turn(LEFT);
						if (horizontal_lines[posY][posX]) { turn(LEFT); }
						break;
					
					case LEFTX:

						if (horizontal_lines[posY][posX] == 0) { turn(LEFT); }
						else if (vertical_lines[posY][1] == 1) { turn(RIGHT); }

						break;

				}

				break;

			case WEST:

				switch (posX) {

					case MIDDLEX:
						
						if (horizontal_lines[posY][posX] == 0) { turn(RIGHT); }
						else if (vertical_lines[posY][1] == 1) { turn(LEFT); }
						break;

					case RIGHTX: 

						if (horizontal_lines[posY][posX] == 0) { turn(RIGHT); }
						else if (vertical_lines[posY][0] == 1) { turn(LEFT); }

						break;
					
					case LEFTX:

						turn(RIGHT);
						if (horizontal_lines[posY][posX]) { turn(RIGHT); }
						break;

				}

				break;
			
			case SOUTH:

				if (isDeadEnd) { nbDeadMove++; }

				switch (posX) {

					case MIDDLEX:
						
						if (vertical_lines[posY][1] == 0) { turn(RIGHT); }
						else if (vertical_lines[posY][0] == 0) { turn(LEFT);} 
						break;

					case RIGHTX: 

						if (vertical_lines[posY][0] == 0) { turn(RIGHT); }
						break;
					
					case LEFTX:

						if (vertical_lines[posY][1] == 0) { turn(LEFT); }
						break;

				}

				break;

		}

		if (posY < 9 && (getRightProx() || getLeftProx())) { advanceUnit(); }	

	}

	/*

	if(comeBack == false){
		if(detectFrequency()){go = true; getInPosition();}
	}
	if(go == true && comeBack == false){
		if(getRightProx()==0||getLeftProx()==0){
			if(posY<9){
				switch (posX)
				{
					case MIDDLEX:
						turn(LEFT);
						if(getRightProx()==0||getLeftProx()==0){
							oneEighty(RIGHT);
							movementTab[mCnt++] = 2;
							advanceUnit();
							movementTab[mCnt++] = 1;
							turn(LEFT);
							movementTab[mCnt++] = 3;
							posX = RIGHTX;
						}
						else{
							movementTab[mCnt++] = 3;
							advanceUnit();
							movementTab[mCnt++] = 1;
							turn(RIGHT);
							movementTab[mCnt++] = 2;
							posX = LEFTX;
							if(getRightProx()==0||getLeftProx()==0){
								turn(RIGHT);
								for(uint8_t i = 0; i < 2; i++){advanceUnit();}
								turn(LEFT);
								posX = RIGHTX;

								movementTab[mCnt-3] = 2;
								movementTab[mCnt-2] = 1;
								movementTab[mCnt-1] = 3;
							}
						}
						break;
					case LEFTX:
						turn(RIGHT);
						movementTab[mCnt++] = 2;
						advanceUnit();
						movementTab[mCnt++] = 1;
						turn(LEFT);
						movementTab[mCnt++] = 3;
						posX = MIDDLEX;
						if(getRightProx()==0||getLeftProx()==0){
							turn(RIGHT);
							movementTab[--mCnt] = 0;
							if(getRightProx()==0||getLeftProx()==0){
								turn(RIGHT);
								movementTab[mCnt++] = 2;
								for(uint8_t i = 0; i < 2; i++){advanceUnit();movementTab[mCnt++] = 1;}
								posY-=2;
								turn(LEFT);
								movementTab[mCnt++] = 3;
								advanceUnit();
								movementTab[mCnt++] = 1;
								turn(LEFT);
								movementTab[mCnt++] = 3;
								posX = RIGHTX;
							}
							else{
								advanceUnit();
								movementTab[mCnt++] = 1;
								turn(LEFT);
								movementTab[mCnt++] = 3;
								posX = RIGHTX;
							}
						}
						break;
					case RIGHTX:
						turn(LEFT);
						movementTab[mCnt++] = 3;
						advanceUnit();
						movementTab[mCnt++] = 1;
						turn(RIGHT);
						movementTab[mCnt++] = 2;
						posX = MIDDLEX;
						if(getRightProx()==0||getLeftProx()==0){
							turn(LEFT);
							movementTab[--mCnt] = 0;
							if(getRightProx()==0||getLeftProx()==0){
								turn(LEFT);
								movementTab[mCnt++] = 3;
								for(uint8_t i = 0; i < 2; i++){advanceUnit();movementTab[mCnt++] = 1;}
								posY-=2;
								turn(RIGHT);
								movementTab[mCnt++] = 2;
								advanceUnit();
								movementTab[mCnt++] = 1;
								turn(RIGHT);
								movementTab[mCnt++] = 2;
								posX = LEFTX;
							}
							else{
								advanceUnit();
								movementTab[mCnt++] = 1;
								turn(RIGHT);
								movementTab[mCnt++] = 2;
								posX = LEFTX;
							}
						}
						break;
				}
			}
		}
		else{
			if(posY < 9){advanceUnit(); posY++; movementTab[mCnt]=1;mCnt++;}
			else{go = false; stop(); posY = 9; posX = MIDDLEX; comeBack = true;}
		}
	}
	if(comeBack == true)
	{
		if(detectFrequency()){goBack();}
	}

	*/
	delay(100);
}
