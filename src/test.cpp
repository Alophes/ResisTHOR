#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"
#include "lcd.h"

#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 0
#define TESTMOVEMENT 0
#define TESTLECTEURLCD 1
#define TESTRFID 0
#define TESTCAPTEURCOULEUR 0
#define TESTSDCARD 0

void test(AllStruct *allStruct)
{

    State *state = allStruct->state;
    Pin pin = allStruct->pin;

    if (TEST_LECTUREPARCOUR)
    {
        readCommand(state->movement);
    }

    if (TEST_FAIREPARCOUR)
    {
        readCommand(state->movement);
        moving(state->movement, state->scAnswer, allStruct);
        delay(1000);
        returnToBase(state->movement, state->scAnswer, allStruct);
    }

    if (TESTMOVEMENT)
    {
        testMovement(allStruct);
    }

    if (TESTLECTEURLCD)
    {

        Serial.println("=================TEST LCD BEGIN=================");
        printLCD(MENU);
        delay(5000);
        Serial.println("=================TEST LCD END=================");
    }

    if (TESTRFID)
    {
        rfidPrintCommand(rfidRead());
    }

    if (TESTCAPTEURCOULEUR){

        Serial.println("================TEST CAPTEUR DE COULEUR BEGIN===============");
        while(1){
            if(readRIFD() == FORWARD){
                delay(250);
                break;
            }
            delay(100);
        }

        int color = detectColor();

        switch (color){

            case BLEU:
                Serial.println("BLEU");
                break;

            case NOIR:
                Serial.println("NOIR");
                break;
            
            case ROUGE:
                Serial.println("ROUGE");
                break;

            case BACHE:
                Serial.println("BACHE");
                break;

        }
        Serial.println("================TEST CAPTEUR DE COULEUR END===============");
    }

    if (TESTSDCARD){

        choseParkour(allStruct);
        while(1){
            delay(500);
            Serial.println(ENCODER_Read(0));
        }
    }
    
}