#include <Arduino.h>
#include <LibRobus.h>

#define UTIL_H

#define FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define START 4
#define STOP 5

#define DETECT 1
#define NODETECT 0

#define FRONT 2
#define REAR 3
#define LEFT 0
#define RIGHT 1

#define AccCALIBRATION 1
#define ForCALIBRATION 1
#define DecCALIBRATION 1

#define BLEU 0
#define VERT 1
#define ROUGE 2
#define NOIR 3

#define MENU 0
#define CHOSEQUESTION 1
#define READCOMMAND 2
#define MOVING 3
#define SADFACE 4
#define HAPPYFACE 5


struct state
{
	char question[128];
	int questionNumber;
	int realAnswer;
	int movement[100];
	int nbAnswer;	 // le nombre de de réponse qu'il y a
	int scAnswer; // les réponse lue (scanner) en tableau
	int nbOfMovement;

	int detectRight;
	int detectLeft;
	int SDInitialized;

	int start;

	int comingBack; // = à 1 quand il est entrain de revenir
};

typedef struct state State; // en gros juste besoin d'écrire State au lieu de struck state pour call la structure

struct forwardParam
{

	int nbIteration = 10;
	int delayIteration = 25;
	int breakDelay = 0;
};

typedef struct forwardParam ForwardParam;

struct pin
{
	int sdReader = 11; 

	int capDroite = 44;
	int capGauche = 45;

	uint8_t potentiometerForward = A0;
	uint8_t potentiometerTurnRight = A1;
	uint8_t potentiometerTurnLeft = A2;
};

typedef struct pin Pin;

struct pulse
{
	int right;
	int left;
	int rightCnt = 0;
	int leftCnt = 0;
	int rightPast;
	int leftPast;
};

typedef struct pulse Pulse;

struct speed
{

	float forwardRight;
	float forwardLeft;
	float accelerationRight;
	float accelerationLeft;
	float decelerationRight;
	float decelerationLeft;
};

typedef struct speed Speed;

struct vitesseRobotA
{ // ajuster les vitesses du robot A

	float forwardR = 0.5;	   // vitesse d'un moteur de base
	float forwardL = 0.507750; // vitesse d'un moteur de base

	float accelerationR = 0.5;	   // vitesse d'accélération
	float accelerationL = 0.501500; // vitesse d'un moteur de base

	float decelerationR = 0.5;		// vitesse d'un moteur de base
	float decelerationL = 0.489510; // vitesse de décélération
};
typedef struct vitesseRobotA VitesseRobotA;

struct vitesseRobotB
{ // ajuster les vitesses du robot B

	float forwardR = 0.5;	   // vitesse d'un moteur de base
	float forwardL = 0.497233; // vitesse d'un moteur de base

	float accelerationR = 0.5;		// vitesse d'accélération
	float accelerationL = 0.502511; // vitesse d'un moteur de base

	float decelerationR = 0.5;		// vitesse d'un moteur de base
	float decelerationL = 0.490380; // vitesse de décélération
};
typedef struct vitesseRobotB VitesseRobotB;

struct basicSettings
{

	char robot = 'A'; // changer selon le robot que vous utilisez

	VitesseRobotA speedRobotA;
	VitesseRobotB speedRobotB;

	float AccKP = 0.00005; // coefficient de correction par pondération
	float KP = 0.000005;

	int ENCODER_LEFT = 0;
	int ENCODER_RIGHT = 1;

	int MOTOR_LEFT = 0;
	int MOTOR_RIGHT = 1;

	float K_ENCODEUR = 25.13 / 3500;

	char valeurAffichage; // ici c'est seulement pour créer un pointeur
	char affichage = 'Y';

	int CALIBRATEMOTORS = 0;

	int maxQuestion = 5;
};

typedef struct basicSettings BasicSettings;

struct allstruct
{
	State *state;
	Speed *speed;
	Pulse *pulse;
	Speed *initialSpeed;
	BasicSettings baseSet;
	Pin pin;
};

typedef struct allstruct AllStruct;

// initVariables.c
Pulse *initPulse(); // isation des pulses
Speed *initSpeed(BasicSettings baseSet);
State *initState();
AllStruct *initAllStruct(BasicSettings baseSet, Pin pin);

// util.c
int readRIFD();
void choseParkour(AllStruct *allstruct);
void readCommand(AllStruct *allStruct);
int moving(int movement[100], int scAnswer, AllStruct *allstruct);
int verifieAnswer(int realAnswer, int scAnswer);
void returnToBase(int movement[100], AllStruct *allstruct);
void detecteurProximite(State *state, Pin pin);
int detectColor();
int stoppingCriteria(AllStruct *allStruct);

void SDInit(State *state, Pin pin);
void loadQuestion(State *state, Pin pin);

// movement.c
void forward(AllStruct *allStruct); // PID et avancer le robot
void stopMotors(AllStruct *allStruct);
void motorsAccelerate(AllStruct *allStruct); // accélération du robot
void readPulse(AllStruct *allStruct);		 // lit les pulses
void accCalibration(AllStruct *allStruct);
void forwardCalibration(AllStruct *allStruct);
void decelatationCalibration(AllStruct *allStruct);
void motorCalibration(AllStruct *allStruct); // calibration des moteurs
void turn(int direction, Pin pin);
void testMovement(AllStruct *allstruct);

//test.cpp
void test(AllStruct *allStruct);