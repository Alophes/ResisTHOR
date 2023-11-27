#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include "rfid.h"
#include "lcd.h"

#define TEST_LECTUREPARCOUR 0
#define TEST_FAIREPARCOUR 0
#define TESTMOVEMENT 0
#define TESTLECTEURLCD 0
#define TESTRFID 0

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
        uint8_t smiley[8] = {
            B01010,
            B01010,
            B01010,
            B00000,
            B10001,
            B01110,
            B00000,
            B00000};

        createCustomChar(0, smiley);
        lcdSetPos(0, 0);
        lcdPuts("bonjour");
        delay(2000);
        lcdClear();
        Serial.println("=================TEST LCD END=================");
    }

    if (TESTRFID)
    {
        rfidPrintCommand(rfidRead());
    }
}