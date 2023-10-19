#include <Arduino.h>
#include <LibRobus.h>

struct pulse {
  uint16_t right;
  uint16_t left;
  uint16_t rightCnt = 0;
  uint16_t leftCnt = 0;
  uint16_t rightPast;
  uint16_t leftPast;
};

typedef struct pulse Pulse;

struct state {
  int coordinates[2];
  
  int detectRight;
  int detectLeft;

  int moving;

};

typedef struct state State;

struct speed {

  float motorRight;
  float motorLeft;
};

typedef struct speed Speed;

struct basicSettings {

    float speed_ini = 0.5; //vitesse d'un moteur de base
    float KP = 0.00001; //coefficient de correction par pondération

    int ENCODER_LEFT = 0;
    int ENCODER_RIGHT = 1;

    int MOTOR_LEFT = 0;
    int MOTOR_RIGHT = 1;

    float K_ENCODEUR = 25.13/3500;
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

  float motorRIGHT;
  float motorLEFT;

};

typedef struct initialspeed InitialSpeed;