#include <avr/io.h>
#include <string.h>
#include <Arduino.h>

#define LCD_RS_INIT()    DDRA  |= (1<<1)
#define LCD_RS_ON()      PORTA |= (1<<1)
#define LCD_RS_OFF()     PORTA &= ~(1<<1)

#define LCD_E_INIT()     DDRA  |= (1<<3)
#define LCD_E_ON()       PORTA |= (1<<3)
#define LCD_E_OFF()      PORTA &= ~(1<<3)

#define LCD_DB4_INIT()   DDRA  |= (1<<5)
#define LCD_DB4_ON()     PORTA |= (1<<5)
#define LCD_DB4_OFF()    PORTA &= ~(1<<5)

#define LCD_DB5_INIT()   DDRA  |= (1<<7)
#define LCD_DB5_ON()     PORTA |= (1<<7)
#define LCD_DB5_OFF()    PORTA &= ~(1<<7)

#define LCD_DB6_INIT()   DDRC  |= (1<<6)
#define LCD_DB6_ON()     PORTC |= (1<<6)
#define LCD_DB6_OFF()    PORTC &= ~(1<<6)

#define LCD_DB7_INIT()   DDRC  |= (1<<4)
#define LCD_DB7_ON()     PORTC |= (1<<4)
#define LCD_DB7_OFF()    PORTC &= ~(1<<4)

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