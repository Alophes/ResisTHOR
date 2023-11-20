#include <stdio.h>
#include "util.h"
#include "math.h"
#include <Arduino.h>
#include "Adafruit_TCS34725.h"


#define LCD_RS_INIT()    DDRB  |= (1<<0)
#define LCD_RS_ON()      PORTB |= (1<<0)
#define LCD_RS_OFF()     PORTB &= ~(1<<0)

#define LCD_E_INIT()     DDRB  |= (1<<2)
#define LCD_E_ON()       PORTB |= (1<<2)
#define LCD_E_OFF()      PORTB &= ~(1<<2)

#define LCD_DB4_INIT()   DDRL  |= (1<<0)
#define LCD_DB4_ON()     PORTL |= (1<<0)
#define LCD_DB4_OFF()    PORTL &= ~(1<<0)

#define LCD_DB5_INIT()   DDRL  |= (1<<2)
#define LCD_DB5_ON()     PORTL |= (1<<2)
#define LCD_DB5_OFF()    PORTL &= ~(1<<2)

#define LCD_DB6_INIT()   DDRL  |= (1<<4)
#define LCD_DB6_ON()     PORTL |= (1<<4)
#define LCD_DB6_OFF()    PORTL &= ~(1<<4)

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
void lcdTexte(uint8_t POSX, uint8_t POSY, char * text);

/**
 * @brief     effacer tout le lcd
 * 
 * 
 * @return    void
 */
void lcdClear();

void setup() {
	lcdInit();
	delay(1000);
	lcdTexte(0,0,"salut");

}


/**
 * @brief     fonction qui permet d'ecrire les 4 bits les moins significatifs
 * 
 * @param     data paramettre a ecrire
 * @param     rs parametre qui dicte si c'est une instruction (0) de la data (1)
 * 
 * @return    void
 */
void _lcdWrite4Bits( uint8_t data, uint8_t rs );
/**
 * @brief     fonction qui permet d'ecrire 4 bits les plus significatifs
 * 
 * @param     data paramettre a ecrire
 * @param     rs parametre qui dicte si c'est une instruction (0) de la data (1)
 * 
 * @return    void
 */
void _lcdWrite8Bits( uint8_t data, uint8_t rs );

void _lcdWrite4Bits( uint8_t data, uint8_t rs )
{
	if(rs)
		LCD_RS_ON();
	else
		LCD_RS_OFF();

	LCD_E_ON();

	if(data & 1)
		LCD_DB4_ON();
	else
		LCD_DB4_OFF();

	if(data & 2)
		LCD_DB5_ON();
	else
		LCD_DB5_OFF();
	
	if(data & 4)
		LCD_DB6_ON();
	else
		LCD_DB6_OFF();
	
	if(data & 8)
		LCD_DB7_ON();
	else
		LCD_DB7_OFF();
	
	delayMicroseconds(1);
	
	LCD_E_OFF();
	
	delayMicroseconds(1);
}

void _lcdWrite8Bits( uint8_t data, uint8_t rs )
{
	_lcdWrite4Bits(data>>4, rs);
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
	
	_lcdWrite4Bits(2, 0);       // mode 4 bits active
	delay(1);
	
	_lcdWrite8Bits(0x28, 0);    // mode 4 bits  -  2 lignes
	_lcdWrite8Bits(0x08, 0);    // LCD a OFF
	_lcdWrite8Bits(0x01, 0);    // Clear LCD
	
	delay(2);
	
	_lcdWrite8Bits(0x06, 0);    // Incremente vers la gauche
	//_lcdWrite8Bits(0x0F, 0);    // LCD a ON, Curseur a ON qui clignote
	_lcdWrite8Bits(0x0C, 0);    // LCD a ON, Curseur a OFF
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
void lcdPuts(char * str)
{
	uint8_t index = 0;
	while(str[index] && index < 255)
	{
		lcdPutc(str[index]);
		index ++;
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
		uint8_t cmd = 0x80;    // commande pour positionner adresse DDRAM
		if (y)
		{
    	    cmd |= (1<<6);     // positionner adresse a 0x40 pour la ligne 2
		}
		cmd |= (x & 0x0F);
		_lcdWrite8Bits(cmd, 0);
}

/**
 * @brief     faire defiler la string de gauche a droite
 * 
 * @param     POSX position horizontal de depart du string
 * @param     POSY position vertical du string
 * @param     text text to print
 * 
 * @return    void
 */
void lcdTexte(uint8_t POSX, uint8_t POSY, char * text)
{
	lcdClear();
	lcdSetPos(POSX, POSY);
	lcdPuts(text);
    uint8_t longueur = strlen(text);
	
    
	while (POSX <= (16-longueur))
	{
		lcdClear();
		lcdSetPos(POSX, POSY);
		lcdPuts(text);
		
		POSX++;
		delay(500);

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

void loop() {
	delay(100);
}

	