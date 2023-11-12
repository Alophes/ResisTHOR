#include <Arduino.h>
#include <LibRobus.h>

int CALIBRATEMOTORS = 1;
#define AccCALIBRATION 1
#define ForCALIBRATION 1
#define DecCALIBRATION 1

//couleur
#define BLEU 1
#define VERT 2
#define JAUNE 3
#define ROUGE 4


struct pulse {
  int right;
  int left;
  int rightCnt = 0;
  int leftCnt = 0;
  int rightPast;
  int leftPast;
};

typedef struct pulse Pulse;

struct state {
  int coordinates[2];
  
  int detectRight;
  
  int detectLeft;

  int moving;

  int color;

  int begin;

};

typedef struct state State;

struct speed {

  float forwardRight;
  float forwardLeft;
  float accelerationRight;
  float accelerationLeft;
  float decelerationRight;
  float decelerationLeft;

};

typedef struct speed Speed;

struct vitesseRobotA{ //ajuster les vitesses du robot A
    
    float forwardR = 0.5; //vitesse d'un moteur de base
    float forwardL = 0.501115; //vitesse d'un moteur de base

    float accelerationR = 0.5; //vitesse d'accélération
    float accelerationL = 0.499950; //vitesse d'un moteur de base

    float decelerationR = 0.5; //vitesse d'un moteur de base
    float decelerationL = 0.489510; //vitesse de décélération
};
typedef struct vitesseRobotA VitesseRobotA;

struct vitesseRobotB{ //ajuster les vitesses du robot B
    
    float forwardR = 0.5; //vitesse d'un moteur de base
    float forwardL = 0.501115; //vitesse d'un moteur de base

    float accelerationR = 0.5; //vitesse d'accélération
    float accelerationL = 0.499950; //vitesse d'un moteur de base

    float decelerationR = 0.5; //vitesse d'un moteur de base
    float decelerationL = 0.489510; //vitesse de décélération
};
typedef struct vitesseRobotB VitesseRobotB;
struct basicSettings {
  
  char robot = 'B'; //changer selon le robot que vous utilisez
  
  VitesseRobotA speedRobotA;
  VitesseRobotB speedRobotB;

  float AccKP = 0.00001; //coefficient de correction par pondération
  float KP = 0.00001;

  int ENCODER_LEFT = 0;
  int ENCODER_RIGHT = 1;

  int MOTOR_LEFT = 0;
  int MOTOR_RIGHT = 1;

  float K_ENCODEUR = 25.13/3500;

  char valeurAffichage; // ici c'est seulement pour créer un pointeur
  char *affichage = &valeurAffichage;


};

typedef struct basicSettings BasicSettings;

struct pin {

    int capDroite = 44;
    int led_capDroite = 45;

    int capGauche = 42;
    int led_capGauche = 43;
};

typedef struct pin Pin;

struct initialspeed{

  float forwardRight;
  float forwardLeft;
  float accelerationRight;
  float accelerationLeft;
  float decelerationRight;
  float decelerationLeft;

};

typedef struct initialspeed InitialSpeed;

//initVariables.c
State *initState(); // isation des états
Pulse *initPulse(); // isation des pulses
Speed *initSpeed();

//motors.c
void forward(int colorToFollow, State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed); //PID et avancer le robot
void stopMotors(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
void motorsAccelerate(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed); // accélération du robot
void readPulse(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed); // lit les pulses
void accCalibration(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
void forwardCalibration(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
void decelatationCalibration(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
void motorCalibration(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed); // calibration des moteurs

//util.c
void printState(); // affiche les données 
int detecteurCouleur(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
void detecteurProximite(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
float getAmbient(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
float getFrequency(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);
int detectFrequency(State *state, Speed *speed, Pulse *pulse, InitialSpeed *initialSpeed);



