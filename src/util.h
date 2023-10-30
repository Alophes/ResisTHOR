#include <Arduino.h>
#include <LibRobus.h>


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

  float motorRight;
  float motorLeft;
  float motorRightAcc;
  float motorLeftAcc;
  float motorRightDec;
  float motorLeftDec;

};

typedef struct initialspeed InitialSpeed;

