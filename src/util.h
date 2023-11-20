#include <Arduino.h>
#include <LibRobus.h>


#define FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define SCAN 4

int readRIFD();
int choseParkour();
void readCommand(int movement[100]);
void moving(int movement[100],int scAnswer[5]);
void forward();
void turnLeft();
void turnRight();
int verifieAnswer(int reponse[5], int nbAswer, int scAnswers[5]);
int scan();
void returnToBase();

struct state{
    int question;
    int bonneRéponse;
    int movement[100];
    int reponse[5];
    int nbAnswer; //le nombre de de réponse qu'il y a
    int scAnswer[5]; //les réponse lue (scanner) en tableau
};

typedef struct state State; //en gros juste besoin d'écrire State au lieu de struck state pour call la structure