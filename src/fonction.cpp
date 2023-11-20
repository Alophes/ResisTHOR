#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"

int readRIFD(){
    //rajouter la fonction

    while(1){

        // tant que ya pas lecture rfid, continu

        //if rifd != 0 break
    }
    return 1; //(rifd);
}


int choseParkour(){
    int puce;
    puce = readRIFD();
    return puce;
}

void readCommand(int movement[100]){
    int i = 0;
    while (1){
        movement[i] = readRIFD();
        if (movement[i] == SCAN) {
            break;
        }
        i++;
    }
}

void moving(int movement[100], int scAnswer[5]){

    int nbOfScan = 0;
    int i = 0;
    while(movement[i] != '\0')
    {
        switch(movement[i]){

            case FORWARD:
                forward();
            case TURNLEFT:
                turnLeft();
            case TURNRIGHT:
                turnRight();
            case SCAN:
                scAnswer[nbOfScan] = scan();
                nbOfScan++;
        }
        i++;
    }
}



int verifieAnswer(int reponse[5], int nbAnswer, int scAnswers[5]){
    for (int i =0; i <= nbAnswer; i++)
    {
        if (reponse[5] != scAnswers[5])
        {
            return 0;
        }
    }
    return 1;
}

void returnToBase()
{
    return;
}

