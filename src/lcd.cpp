#include "lcd.h"

#ifndef UTIL_H
#include "util.h"
#endif

/**
 * @brief     fonction qui permet d'ecrire les 4 bits les moins significatifs
 *
 * @param     data paramettre a ecrire
 * @param     rs parametre qui dicte si c'est une instruction (0) de la data (1)
 *
 * @return    void
 */
void _lcdWrite4Bits(uint8_t data, uint8_t rs);
/**
 * @brief     fonction qui permet d'ecrire 4 bits les plus significatifs
 *
 * @param     data paramettre a ecrire
 * @param     rs parametre qui dicte si c'est une instruction (0) de la data (1)
 *
 * @return    void
 */
void _lcdWrite8Bits(uint8_t data, uint8_t rs);
void _lcdWrite4Bits(uint8_t data, uint8_t rs)
{
    if (rs)
        LCD_RS_ON();
    else
        LCD_RS_OFF();

    LCD_E_ON();

    if (data & 1)
        LCD_DB4_ON();
    else
        LCD_DB4_OFF();

    if (data & 2)
        LCD_DB5_ON();
    else
        LCD_DB5_OFF();

    if (data & 4)
        LCD_DB6_ON();
    else
        LCD_DB6_OFF();

    if (data & 8)
        LCD_DB7_ON();
    else
        LCD_DB7_OFF();

    delayMicroseconds(1);

    LCD_E_OFF();

    delayMicroseconds(1);
}
void _lcdWrite8Bits(uint8_t data, uint8_t rs)
{
    _lcdWrite4Bits(data >> 4, rs);
    _lcdWrite4Bits(data, rs);
    delayMicroseconds(40);
}
/**
 * @brief     Initialiser le lcd en mode 4 bits, 2 lignes, LCD a ON, Curseur a OFF
 *
 *
 * @return    void
 */
void lcdInit()
{
    LCD_RS_INIT();
    LCD_E_INIT();
    LCD_DB4_INIT();
    LCD_DB5_INIT();
    LCD_DB6_INIT();
    LCD_DB7_INIT();

    delay(20);

    _lcdWrite4Bits(3, 0);
    delay(5);

    _lcdWrite4Bits(3, 0);
    delay(1);

    _lcdWrite4Bits(3, 0);
    delay(1);

    _lcdWrite4Bits(2, 0); // mode 4 bits active
    delay(1);

    _lcdWrite8Bits(0x28, 0); // mode 4 bits  -  2 lignes
    _lcdWrite8Bits(0x08, 0); // LCD a OFF
    _lcdWrite8Bits(0x01, 0); // Clear LCD

    delay(2);

    _lcdWrite8Bits(0x06, 0); // Incremente vers la gauche
    //_lcdWrite8Bits(0x0F, 0); // LCD a ON, Curseur a ON qui clignote
    _lcdWrite8Bits(0x0C, 0); // LCD a ON, Curseur a OFF
}
/**
 * @brief     Ecrire un seul caractere
 *
 * @param     c Caractere a ecrire
 *
 * @return    void
 */
void lcdPutc(char c)
{
    _lcdWrite8Bits(c, 1);
}
/**
 * @brief     ecrire une chaine de caractere
 *
 * @param     str Chaine de caractere a ecrire
 *
 * @return    void
 */
void lcdPuts(char *str)
{
    uint8_t index = 0;
    while (str[index] && index < 255)
    {
        lcdPutc(str[index]);
        index++;
    }
}
/**
 * @brief     definir la position de la chaine de caractere dans le lcd
 *
 * @param     x position horizontal du string
 * @param     y position vertical du string
 *
 * @return    void
 */
void lcdSetPos(uint8_t x, uint8_t y)
{
    uint8_t cmd = 0x80; // commande pour positionner adresse DDRAM
    if (y)
    {
        cmd |= (1 << 6); // positionner adresse a 0x40 pour la ligne 2
    }
    cmd |= (x & 0x0F);
    _lcdWrite8Bits(cmd, 0);
}
/**
 * @brief     faire defiler la string de gauche a droite
 *
 * @param     posXStart position horizontal de depart du string
 * @param     posY position vertical du string
 * @param     text text to print
 *
 * @return    void
 */

void lcdLongTexte(uint8_t posXStart, uint8_t posXEnd, uint8_t posY, char *text)
{

    // Your message
    String message = text;
    uint8_t lenght = strlen(text);
    char outMsg[1024];
    // Gradually appear from the right
    for (int i = 15; i >= 0; i--)
    {
        lcdSetPos(i, 0);
        String leftCut = message.substring(0, 16 - i);
        strcpy(outMsg, leftCut.c_str());
        lcdPuts(outMsg);
        delay(200);
        lcdClear();
    }

    // Slide to the left
    for (int i = 0; i < lenght; i++)
    {
        lcdSetPos(0, 0);
        String leftCut = message.substring(i, lenght);
        String rightCut = leftCut.substring(0, 16);
        strcpy(outMsg, rightCut.c_str());
        lcdPuts(outMsg);
        delay(200);
        lcdClear();
    }
}
/**
 * @brief     effacer tout le lcd
 *
 *
 * @return    void
 */
void lcdClear()
{
    _lcdWrite8Bits(0x01, 0);
    delay(2);
}
void createCustomChar(int location, uint8_t character[])
{
    LCD_RS_OFF();                              // Set RS to command mode
    _lcdWrite8Bits(0x40 | (location << 3), 0); // Set CGRAM address to define custom character

    LCD_RS_ON(); // Set RS to data mode
    for (uint8_t i = 0; i < 8; i++)
    {
        _lcdWrite8Bits(character[i], 1); // Send 8 bytes of custom character data
    }
}

void printLCD(int whatToPrint, AllStruct *allStruct)
{

    State *state = allStruct->state;
    BasicSettings baseSet = allStruct->baseSet;

    uint8_t schemaCarre[8] = {
        B10101,
        B01010,
        B10101,
        B01010,
        B10101,
        B01010,
        B10101,
        B01010};

    uint8_t hammer[8] = {
        B11111,
        B10001,
        B11111,
        B00100,
        B00100,
        B00100,
        B00100,
        B00100};

    uint8_t eclairLeft[8] = {
        B00001,
        B00010,
        B00100,
        B01111,
        B11110,
        B00100,
        B01000,
        B10000};

    uint8_t eclairRight[8] = {
        B10000,
        B01000,
        B00100,
        B11110,
        B01111,
        B00100,
        B00010,
        B00001};

    uint8_t forward[8] = {
        B00100,
        B01110,
        B10101,
        B00100,
        B00100,
        B00100,
        B00100,
        B00100};

    uint8_t turnLeft[8] = {
        B00000,
        B00100,
        B01000,
        B11111,
        B11111,
        B01000,
        B00100,
        B00000};

    uint8_t turnRight[8] = {
        B00000,
        B00100,
        B00010,
        B11111,
        B11111,
        B00010,
        B00100,
        B00000};

    uint8_t sadFaceEye[8] = {
        B00000,
        B11111,
        B00000,
        B00000,
        B10001,
        B01110,
        B00000,
        B00000};

    uint8_t sadFaceMidleMouth[8] = {
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B00000,
        B00000,
        B00000};

    uint8_t sadFaceLeftMouth[8] = {
        B00000,
        B00000,
        B00000,
        B00000,
        B01111,
        B10000,
        B00000,
        B00000};

    uint8_t sadFaceRightMouth[8] = {
        B00000,
        B00000,
        B00000,
        B00000,
        B11110,
        B00001,
        B00000,
        B00000};

    uint8_t HappyFaceLeftEye[8] = {
        B00000,
        B01100,
        B10000,
        B00000,
        B01110,
        B10001,
        B00000,
        B00000};

    uint8_t HappyFaceRightEye[8] = {
        B00000,
        B00110,
        B00001,
        B00000,
        B01110,
        B10001,
        B00000,
        B00000};

    uint8_t happyFaceLeftMouth[8] = {
        B00000,
        B00000,
        B11111,
        B10010,
        B10010,
        B01010,
        B00111,
        B00000};

    uint8_t happyFaceRightMouth[8] = {
        B00000,
        B00000,
        B11111,
        B01001,
        B01001,
        B01010,
        B11100,
        B00000};

    uint8_t happyFaceMidleMouth[8] = {
        B00000,
        B00000,
        B11111,
        B10001,
        B10001,
        B10001,
        B11111,
        B00000};

    lcdClear();

    int screen[2][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    if (whatToPrint == MENU)
    {
        int screenMenu[2][16] = {{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2},
                                 {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

        lcdSetPos(3, 0);
        lcdPuts("resisTHOR");
        createCustomChar(12, hammer);
        lcdSetPos(12, 0);
        lcdPutc(12);

        createCustomChar(0, schemaCarre);
        for (int i = 0; i < 2; i++)
        {
            for (int y = 0; y < 16; y++)
            {

                if (screenMenu[i][y] == 0)
                {
                    createCustomChar(0, eclairLeft);
                    lcdSetPos(y, i);
                    lcdPutc(0);
                }

                if (screenMenu[i][y] == 2)
                {
                    createCustomChar(1, eclairRight);
                    lcdSetPos(y, i);
                    lcdPutc(1);
                }
            }
        }
        lcdSetPos(0, 1);
        lcdPuts("DEPART =>");
        return;
    }

    if (whatToPrint == CHOSEQUESTION)
    {

        state->questionNumber = 0;
        while (1)
        {
            lcdClear();
            lcdSetPos(0, 0);
            lcdPuts("No. parcours :");
            lcdSetPos(0, 1);
            lcdPuts("DEPART =>");
            lcdSetPos(15, 0);
            char nbq = '0' + state->questionNumber;
            lcdPutc(nbq);

            int parcourSelected = readRIFD();

            if (parcourSelected == TURNRIGHT)
            {
                state->questionNumber++;
                if (state->questionNumber > baseSet.maxQuestion)
                {
                    state->questionNumber = 0;
                }
            }

            if (parcourSelected == TURNLEFT)
            {
                state->questionNumber--;
                if (state->questionNumber < 0)
                {
                    state->questionNumber = baseSet.maxQuestion;
                }
            }

            if (parcourSelected == START)
            {
                Serial.println("START");
                return;
            }

            else if (parcourSelected != STOP && parcourSelected != TURNRIGHT && parcourSelected != TURNLEFT)
            {
                lcdClear();
                lcdSetPos(0, 0);
                lcdPuts("Droit : + 1");
                lcdSetPos(0, 1);
                lcdPuts("Gauche : - 1");

                delay(2000);
            }
            delay(250);
        }
        return;
    }

    int i = 0;
    if (whatToPrint == READCOMMAND)
    {
        lcdSetPos(0, 0);
        lcdPuts("Mouvements:");
        lcdSetPos(12, 0);
        int y = 11;
        int x = 0;
        while (state->movement[i] != '\0')
        {

            if (y > 15)
            {
                x = 1;

                lcdSetPos((y - 15), x);
                if (state->movement[i] == FORWARD)
                {
                    createCustomChar(13, forward);
                    lcdSetPos(y, x);
                    lcdPutc(13);
                }

                if (state->movement[i] == TURNLEFT)
                {
                    createCustomChar(14, turnLeft);
                    lcdSetPos(y, x);
                    lcdPutc(14);
                }

                if (state->movement[i] == TURNRIGHT)
                {
                    createCustomChar(15, turnRight);
                    lcdSetPos(y, x);
                    lcdPutc(15);
                }
            }

            else
            {

                if (state->movement[i] == FORWARD)
                {
                    createCustomChar(13, forward);
                    lcdSetPos(y, x);
                    lcdPutc(13);
                }

                if (state->movement[i] == TURNLEFT)
                {
                    createCustomChar(14, turnLeft);
                    lcdSetPos(y, x);
                    lcdPutc(14);
                }

                if (state->movement[i] == TURNRIGHT)
                {
                    createCustomChar(15, turnRight);
                    lcdSetPos(y, x);
                    lcdPutc(15);
                }
            }
            i++;
            y++;
        }
        return;
    }

    if (whatToPrint == MOVING)
    {

        lcdClear();
        lcdSetPos(0, 0);
        char texte[33] = {"scanner DEPART pour commencer =>"};
        // lcdLongTexte(0, 0, 0, texte);

        lcdSetPos(0, 0);
        lcdPuts("DEPART =>");
        while (readRIFD() != START)
        {
            delay(100);
        }

        lcdClear();
        char *three[1] = {"3"};
        lcdLongTexte(0, 0, 0, *three);

        char *two[1] = {"2"};
        lcdLongTexte(0, 0, 0, *two);

        char *one[1] = {"1"};
        lcdLongTexte(0, 0, 0, *one);

        lcdSetPos(4, 0);
        lcdPuts("DEPART");

        return;
    }

    if (whatToPrint == SADFACE)
    {

        createCustomChar(0, sadFaceEye);
        lcdSetPos(6, 0);
        lcdPutc(0);

        lcdSetPos(9, 0);
        lcdPutc(0);

        createCustomChar(1, sadFaceLeftMouth);
        lcdSetPos(5, 1);
        lcdPutc(1);

        createCustomChar(2, sadFaceMidleMouth);
        lcdSetPos(6, 1);
        lcdPutc(2);
        lcdSetPos(7, 1);
        lcdPutc(2);
        lcdSetPos(8, 1);
        lcdPutc(2);
        lcdSetPos(9, 1);
        lcdPutc(2);

        createCustomChar(3, sadFaceRightMouth);
        lcdSetPos(10, 1);
        lcdPutc(3);
        return;
    }

    if (whatToPrint == HAPPYFACE)
    {

        createCustomChar(0, HappyFaceLeftEye);
        lcdSetPos(6, 0);
        lcdPutc(0);

        createCustomChar(4, HappyFaceRightEye);
        lcdSetPos(9, 0);
        lcdPutc(4);

        createCustomChar(1, happyFaceLeftMouth);
        lcdSetPos(5, 1);
        lcdPutc(1);

        createCustomChar(2, happyFaceMidleMouth);
        lcdSetPos(6, 1);
        lcdPutc(2);
        lcdSetPos(7, 1);
        lcdPutc(2);
        lcdSetPos(8, 1);
        lcdPutc(2);
        lcdSetPos(9, 1);
        lcdPutc(2);

        createCustomChar(3, happyFaceRightMouth);
        lcdSetPos(10, 1);
        lcdPutc(3);
        return;
    }

    if (whatToPrint == TESTMOVEMENT){
        lcdClear();
        lcdSetPos(0,0);
        lcdPuts("MOUVEMENT");
    }
}
