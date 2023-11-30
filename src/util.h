#include <Arduino.h>
#include <LibRobus.h>

#define UTIL_H

//Paramètre à changer
#define TEST 0
#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 0
#define TESTMOVEMENT 1
#define TESTLECTEURLCD 0
#define TESTRFID 0
#define TESTCAPTEURCOULEUR 0
#define TESTSDCARD 0
#define POTENTIONMETER 0
#define BUZZER 0
#define TESTPID 0

#define AccCALIBRATION 1
#define ForCALIBRATION 0
#define DecCALIBRATION 0

//Paramètre à ne pas touché
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
	int questionNumber; /** @brief Numéro de la question choisi*/
	int realAnswer; /** @brief Bonne réponse*/ 
	int movement[100]; /** @brief Tableau qui garde en mémoire les mouvements*/
	int scAnswer; /** @brief Réponse scanner à la fin de ses mouvement*/
	int nbOfMovement; /** @brief Position du dernier mouvement qu'il a fait. Utile quand il détecte un mur*/

	int detectRight; /** @brief Variable changeante selon si le capteur de proximité droit détecte ou pas (0 = ne détecte pas | 1 = détecte)*/
	int detectLeft; /** @brief Variable changeante selon si le capteur de proximité gauche détecte ou pas (0 = ne détecte pas | 1 = détecte)*/
	int SDInitialized;  /** @brief Variable (non-utilisée) si la carte SD a été initialisé */

	int start; /** @brief Variable pour le retourn au départ (1 = retour au dépaart | 0 = autre)*/

	int comingBack;  /** @brief Variable utilisé pour ne pas qu'il vérifie s'il a des mûrs quand il revient*/
};

typedef struct state State; /** @brief Struct gardant en mémoire des variables de l'état du robot*/

struct forwardParam
{

	int nbIteration = 10;
	int delayIteration = 25;
	int breakDelay = 0;
};

typedef struct forwardParam ForwardParam;  /** @brief Paramètre pour la fonction forward*/

struct pin
{
	int sdReader = 11;

	int capDroite = 44;
	int capGauche = 45;

	uint8_t potentiometerForward = A1;
	uint8_t potentiometerTurnRight = A2;
	uint8_t potentiometerTurnLeft = A0;
};

typedef struct pin Pin; /** @brief Struct contenant les numéros de pins*/

struct pulse
{
	int right;
	int left;
	int rightCnt = 0;
	int leftCnt = 0;
	int rightPast;
	int leftPast;
};

typedef struct pulse Pulse; /** @brief Struct contenant les valeurs des pulses*/

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

	float forwardR = 0.5;	   /** @brief vitesse d'avancement du moteur de droit du robot*/ 
	float forwardL = 0.508550; /** @brief vitesse d'avancement du moteur de gauche du robot*/ 

	float accelerationR = 0.5;		/** @brief vitesse d'accélération du moteur droit*/ 
	float accelerationL = 0.52;     /** @brief vitesse d'accélération du moteur gauche*/ 

	float decelerationR = 0.5;		 /** @brief vitesse de décélération du moteur droit*/ 
	float decelerationL = 0.489805;  /** @brief vitesse de décélération du moteur gauche*/ 
};
typedef struct vitesseRobotA VitesseRobotA;

struct vitesseRobotB
{ // ajuster les vitesses du robot B

	float forwardR = 0.5;	   /** @brief vitesse d'avancement du moteur de droit du robot*/ 
	float forwardL = 0.497233; /** @brief vitesse d'avancement du moteur de gauche du robot*/ 

	float accelerationR = 0.5;		/** @brief vitesse d'accélération du moteur droit*/ 
	float accelerationL = 0.51;     /** @brief vitesse d'accélération du moteur gauche*/ 

	float decelerationR = 0.5;		/** @brief vitesse de décélération du moteur droit*/ 
	float decelerationL = 0.490380; /** @brief vitesse de décélération du moteur gauche*/ 
};
typedef struct vitesseRobotB VitesseRobotB;

struct basicSettings
{

	char robot = 'A'; /** @brief changer selon le robot que vous utilisez*/  

	VitesseRobotA speedRobotA;
	VitesseRobotB speedRobotB;

	float AccKP = 0.0001; /** @brief coefficient de correction par pondération pour l'accélération et décélération*/ 
	float KP = 0.000005;  /** @brief coefficient de correction par pondération pour forward*/

	int ENCODER_LEFT = 0;
	int ENCODER_RIGHT = 1;

	int MOTOR_LEFT = 0;
	int MOTOR_RIGHT = 1;

	float K_ENCODEUR = 25.13 / 3500;

	char valeurAffichage; /** @brief ici c'est seulement pour créer un pointeur*/ 
	char affichage = 'Y'; /** @brief à changer si vous voulez afficher les valeurs*/

	int CALIBRATEMOTORS = 0; /** @brief à changer si vous voulez calibrer les moteurs*/

	int maxQuestion = 5; /** @brief nombre question dans le robot*/
};

typedef struct basicSettings BasicSettings; /** @brief Settings de base*/ 

struct allstruct
{
	State *state;
	Speed *speed;
	Pulse *pulse;
	Speed *initialSpeed;
	BasicSettings baseSet;
	Pin pin;
};

typedef struct allstruct AllStruct; /** @brief Struct contenant toutes les structs*/

struct question
{

	int question[6] = {BLEU, BLEU, ROUGE, VERT, BLEU, ROUGE}; 
};

typedef struct question Question; /** @brief réponse au question*/

// initVariables (util.cpp)
Pulse *initPulse(); /** @brief initialisation du struct pulse*/
Speed *initSpeed(BasicSettings baseSet); /** @brief initialisation struct speed*/
State *initState(); /** @brief initialisation struct state*/
AllStruct *initAllStruct(BasicSettings baseSet, Pin pin); /** @brief initialisation du struct allstruct*/

//util.cpp
/** @brief retourne le int de la carte rfid lu*/
int readRIFD(); 
/** @brief fonction pour choisir le parcour/question*/
void choseParkour(AllStruct *allstruct);
/** @brief fonction qui lit les commandes pour programmer le robot*/
void readCommand(AllStruct *allStruct);
/** @brief fonction qui fait bouger le robot*/
int moving(int movement[100], int scAnswer, AllStruct *allstruct);
/** @brief fonction qui vérifie la réponse*/
int verifieAnswer(int realAnswer, int scAnswer);
/** @brief fonction pour retourner à la case départ*/
void returnToBase(int movement[100], AllStruct *allstruct);
/** @brief fonction qui met à jour state->pulse*/
void detecteurProximite(State *state, Pin pin);
/** @brief fonction qui retourne le int d'une couleur*/
int detectColor();
/** @brief fonction qui retourn 1 si le critère d'arrêt est présent, sinon retourne 0*/
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

// test.cpp
void test(AllStruct *allStruct);