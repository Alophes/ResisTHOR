#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#ifndef UTIL_H
#include "util.h"
#endif
#include "rfid.h"
#include "lcd.h"



void test(AllStruct *allStruct)
{

    State *state = allStruct->state;
    Pin pin = allStruct->pin;

    if (TEST_LECTUREPARCOUR)
    {
        readCommand(allStruct);
    }

    if (TEST_FAIREPARCOUR)
    {
        readCommand(allStruct);
        moving(state->movement, state->scAnswer, allStruct);
        delay(1000);
        returnToBase(state->movement, allStruct);
    }

    if (TESTMOVEMENT)
    {
        testMovement(allStruct);
    }

    if (TESTLECTEURLCD)
    {

        Serial.println("=================TEST LCD BEGIN=================");
        /*printLCD(MENU, allStruct);
        delay(5000);
        printLCD(CHOSEQUESTION, allStruct);
        delay(1000);
        readCommand(allStruct);
        printLCD(MOVING, allStruct);*/
        // printLCD(SADFACE, allStruct);
        printLCD(HAPPYFACE, allStruct);

        Serial.println("=================TEST LCD END=================");
    }

    if (TESTRFID)
    {
        rfidPrintCommand(rfidRead());
    }

    if (TESTCAPTEURCOULEUR)
    {

        Serial.println("================TEST CAPTEUR DE COULEUR BEGIN===============");
        while (1)
        {
            if (readRIFD() == FORWARD)
            {
                delay(250);
                break;
            }
            delay(100);
        }

        int color = detectColor();

        switch (color)
        {

        case BLEU:
            Serial.println("BLEU");
            break;

        case NOIR:
            Serial.println("NOIR");
            break;

        case ROUGE:
            Serial.println("ROUGE");
            break;

        case VERT:
            Serial.println("VERT");
            break;
        }
        Serial.println("================TEST CAPTEUR DE COULEUR END===============");
    }

    if (TESTSDCARD)
    {

        choseParkour(allStruct);
        while (1)
        {
            delay(500);
            Serial.println(ENCODER_Read(0));
        }
    }

    if (POTENTIONMETER)
    {

        while (1)
        {
            Serial.println((float)analogRead(pin.potentiometerTurnLeft) / 1023);
            delay(0.250);
        }
    }

    if(TESTPID){

        while(1){

            int choice = readRIFD();


            if(choice == FORWARD){

                motorsAccelerate(allStruct);
                forward(allStruct);
                stopMotors(allStruct);
            }

            if(choice == TURNLEFT){

                motorsAccelerate(allStruct);
                stopMotors(allStruct);
            }
        }
    }
}