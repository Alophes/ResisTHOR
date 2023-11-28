#include <avr/io.h>
#include <string.h>
#include <Arduino.h>
#ifndef UTIL_H
#include "util.h"
#endif

#define LCD

#define MENU 0
#define CHOSEQUESTION 1
#define READCOMMAND 2
#define MOVING 3
#define SADFACE 4
#define HAPPYFACE 5


#define LCD_RS_INIT()    DDRD  |= (1<<7)
#define LCD_RS_ON()      PORTD |= (1<<7)
#define LCD_RS_OFF()     PORTD &= ~(1<<7)

#define LCD_E_INIT()     DDRG  |= (1<<2)
#define LCD_E_ON()       PORTG |= (1<<2)
#define LCD_E_OFF()      PORTG &= ~(1<<2)

#define LCD_DB4_INIT()   DDRG  |= (1<<1)
#define LCD_DB4_ON()     PORTG |= (1<<1)
#define LCD_DB4_OFF()    PORTG &= ~(1<<1)

#define LCD_DB5_INIT()   DDRG  |= (1<<0)
#define LCD_DB5_ON()     PORTG |= (1<<0)
#define LCD_DB5_OFF()    PORTG &= ~(1<<0)

#define LCD_DB6_INIT()   DDRL  |= (1<<7)
#define LCD_DB6_ON()     PORTL |= (1<<7)
#define LCD_DB6_OFF()    PORTL &= ~(1<<7)

#define LCD_DB7_INIT()   DDRL  |= (1<<6)
#define LCD_DB7_ON()     PORTL |= (1<<6)
#define LCD_DB7_OFF()    PORTL &= ~(1<<6)

/**
 * @brief     Initialiser le lcd en mode 4 bits, 2 lignes, LCD a ON, Curseur a OFF
 * 
 * 
 * @return    void
 */
void lcdInit();
/**
 * @brief     Ecrire un seul caractere
 * 
 * @param     c Caractere a ecrire
 * 
 * @return    void
 */
void lcdPutc(char c);
/**
 * @brief     ecrire une chaine de caractere
 * 
 * @param     str Chaine de caractere a ecrire
 * 
 * @return    void
 */
void lcdPuts(char * str);
/**
 * @brief     definir la position de la chaine de caractere dans le lcd
 * 
 * @param     x position horizontal du string
 * @param     y position vertical du string
 * 
 * @return    void
 */
void lcdSetPos(uint8_t x, uint8_t y);
/**
 * @brief     faire defiler la string de gauche a droite dans les deux ligne
 * 
 * @param     POSX position horizontal du string
 * @param     POSY position vertical du string
 * @param     text text to print
 * 
 * @return    void
 */
void lcdLongTexte(uint8_t posXStart, uint8_t posXEnd, uint8_t posY, char * text);
/**
 * @brief     effacer tout le lcd
 * 
 * 
 * @return    void
 */
void lcdClear();
void createCustomChar(int location, uint8_t character[]) ;

void printLCD(int whatToPrint, AllStruct *allStruct);


//resisTHOR

