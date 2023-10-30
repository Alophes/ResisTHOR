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

  float motorRight;
  float motorLeft;
  float motorRightAcc;
  float motorLeftAcc;
  float motorRightDec;
  float motorLeftDec;
};

typedef struct speed Speed;

struct basicSettings {

    float speed_iniR = 0.5; //vitesse d'un moteur de base
    float speed_iniL = 0.501115; //vitesse d'un moteur de base

    float speed_iniAccR = 0.5; //vitesse d'accélération
    float speed_iniAccL = 0.499950; //vitesse d'un moteur de base

    float speed_iniDecR = 0.5; //vitesse d'un moteur de base
    float speed_iniDecL = 0.489510; //vitesse de décélération

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