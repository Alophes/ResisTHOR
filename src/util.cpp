#include <Arduino.h>
#include <LibRobus.h>
#include "math.h"
#include "util.h"
#include <SPI.h>
#include <SD.h>
#include "rfid.h"
#include "Adafruit_TCS34725.h"
#include "lcd.h"

int readRIFD()
{
    // rajouter la fonction
    return rfidReturnCommand(rfidRead());
}

void choseParkour(AllStruct *allstruct)
{

    State *state = allstruct->state;

    int puce;
    Serial.println("==================CHOSEPARKOUR BEGIN===================");

    printLCD(CHOSEQUESTION, allstruct);

    loadQuestion(state, allstruct->pin);

    Serial.print("Question : question");
    Serial.println(state->questionNumber);

    Serial.print("realAnswer : ");
    Serial.print(state->realAnswer);
    Serial.print(" : ");
    switch (state->realAnswer)
    {

    case BLEU:
        Serial.println("BLEU");

    case NOIR:
        Serial.println("NOIR");

    case ROUGE:
        Serial.println("ROUGE");

    case VERT:
        Serial.println("VERT");
    }

    Serial.print("questionNumber : ");
    Serial.println(state->questionNumber);

    Serial.println("==================CHOSEPARKOUR END===================");
}

void readCommand(AllStruct *allStruct)
{
    lcdClear();
    lcdSetPos(0, 0);
    lcdPuts("Mouvements:");

    State *state = allStruct->state;
    int *movement = state->movement;
    Serial.println("Entrez vos valeurs");
    int i = 0;
    while (1)
    {
        movement[i] = readRIFD();
        if(movement[i] != START && movement[i] != STOP)
        {
            movement[i + 1] = '\0';
            i++;
        }

        if (movement[i] == START)
        {
            movement[i + 1] = '\0';
            break;
        }
        
        if(movement[i] == STOP){
            movement[i-1] = '\0';
            i--;
        }

        
        printLCD(READCOMMAND, allStruct);
        delay(500);
        
    }

    int j = 0;
    Serial.print("movement = ");
    while (movement[j] != '\0')
    {
        Serial.print(movement[j]);
        j++;
    }
    Serial.print("\n");
    delay(1000);
}

int moving(int movement[100], int scAnswer, AllStruct *allstruct)
{

    Serial.println("=========================MOVING BEGIN=========================");
    Pin pin = allstruct->pin;

    int nbOfScan = 0;
    int i = 0;
    while (movement[i] != '\0')
    {
        Serial.print("movement [");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(movement[i]);

        if (movement[i] == FORWARD)
        {
            if(stoppingCriteria(allstruct) == 1){
                allstruct->state->scAnswer = NOIR;
                return i;
            }
            Serial.println("I'm going forward");
            motorsAccelerate(allstruct);
            forward(allstruct);
            stopMotors(allstruct);
        }

        if (movement[i] == TURNLEFT)
        {
            Serial.println("I'm turning left");
            turn(LEFT, allstruct->pin);
        }

        if (movement[i] == TURNRIGHT)
        {
            Serial.println("I'm turning Right");
            turn(RIGHT, allstruct->pin);
        }

        if (movement[i] == START)
        {
            AX_BuzzerON(250, 1000);
            allstruct->state->scAnswer = detectColor();
            delay(500);
        }

        i++;
    }
    return i;
    Serial.println("=========================MOVING END=========================");
}

int verifieAnswer(int realAnswer, int scAnswer)
{
    if (scAnswer != realAnswer)
    {
        return 0;
    }
    return 1;
}

void returnToBase(int movement[100], AllStruct *allstruct)
{
    allstruct->state->comingBack = 1;
    State *state = allstruct->state;
    Serial.println("=========================COMING_BACK BEGIN=========================");
    int i = state->nbOfMovement - 1;

    turn(RIGHT, allstruct->pin);
    turn(RIGHT, allstruct->pin);

    Serial.print("Last i = ");
    Serial.println(i);

    while (1)
    {

        Serial.print("movement [");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(movement[i]);

        if (movement[i] == FORWARD)
        {
            Serial.println("I'm going forward");
            motorsAccelerate(allstruct);
            forward(allstruct);
            stopMotors(allstruct);
        }
        if (movement[i] == TURNLEFT)
        {
            Serial.println("I'm turning right");
            turn(RIGHT, allstruct->pin);
        }
        if (movement[i] == TURNRIGHT)
        {
            Serial.println("I'm turning left");
            turn(LEFT, allstruct->pin);
        }

        if (i == 0)
        {
            break;
        }
        i--;
    }

    turn(RIGHT, allstruct->pin);
    turn(RIGHT, allstruct->pin);
    allstruct->state->comingBack = 0;

    Serial.println("=========================COMING_BACK END=========================");
}

void detecteurProximite(State *state, Pin pin)
{

    if (digitalRead(pin.capDroite) == LOW)
    { // Détection à droite
        // digitalWrite(pin.led_capDroite, HIGH); //Allumage du led droit
        state->detectRight = 1;
    }

    else
    {
        // digitalWrite(pin.led_capDroite, HIGH); // Fermeture du led droit
        state->detectRight = 0;
    }

    if (digitalRead(pin.capGauche) == LOW)
    { // Détection à gauche
        // digitalWrite(pin.led_capGauche, HIGH);//Allumage du led gauche
        state->detectLeft = 1;
    }
    else
    {
        // digitalWrite(pin.led_capGauche, HIGH);  // Fermeture du led gauche
        state->detectLeft = 0;
    }
}

void SDInit(State *state, Pin pin)
{
    if (!SD.begin(pin.sdReader))
    {
        Serial.println("no SD card were found");
        state->SDInitialized = false;
        delay(100);
    }
    else
    {
        Serial.println("SD card found\n");
        state->SDInitialized = true;
        delay(100);
    }
}

void loadQuestion(State *state, Pin pin)
{

    /*
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    SDInit(state, pin);


    int *answer;
    if (state->SDInitialized)
    {

        // Variables initialization
        char filename[64];
        char buffer[1024];

        // Generating filename with index of question
        sprintf(filename, "qst%d.txt", state->questionNumber);
        File file = SD.open(filename, FILE_READ);

        // Return if file not found
        if (!file)
        {
            Serial.println("File not found");
            return;
        }

        // Read all file content
        file.read(buffer, 1024);

        // Assign value to variables
        sscanf(buffer, "%[^\n]%d", state->question, answer);
        state->realAnswer = *answer+'0';

        // Close file
        file.close();

    }

    digitalWrite(11, HIGH);
    BoardInit();*/
    Question question;
    state->realAnswer = question.question[state->questionNumber];
}

void InitColor()
{

    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

    if (tcs.begin())
    {
        Serial.println("Found sensor");
    }
    else
    {
        Serial.println("No TCS34725 found ... check your connections");
        while (1)
            ;
    }
}

int detectColor()
{

    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

    const int taille = 10; // Nombre de valeur qu'on utilise pour vérifier
    const int valIdentique = 8;
    uint16_t r, g, b, c; // valeur lue par le capteur (doit rester en uint16_t)

    // Pour le tableau

    int colorRead[taille]; // Tableau des données lues par le capteur pour comparer les couleurs

    while (1)
    {

        for (int i = 0; i < taille; i++)
        {
            tcs.getRawData(&r, &g, &b, &c);

            // delay(1000);

            if (g > b && g > r)
            { // logique de Barin de détection des couleurs
                colorRead[i] = VERT;
                // Serial.println("BACHE");
            }
            else if (r > g && r > b && c < 75)
            {
                colorRead[i] = ROUGE;
                // Serial.println("Rouge");
            }
            else if (c < 50 && r < 20 && g < 20 && b < 20)
            {
                colorRead[i] = NOIR;
                // Serial.println("NOIR");
            }
            else if (b > r && b > g)
            {
                colorRead[i] = BLEU;
                // Serial.println("Bleu");
            }
            else
            {
                colorRead[i] = -1;
                Serial.println("Donnée de merde");
                return NOIR;
            }
        }
        int GREEN = 0, RED = 0, BLACK = 0, BLUE = 0; // Les valeurs seront utilisé pour compter le nombre de fois qu'il a lu la couleur
        int error = 0;                               // Est pour géré les erreurs
        for (int j = 0; j < taille; j++)
        {

            switch (colorRead[j]) // La switch compte le nombre d'occurence d'une couleur
            {
            case VERT:
                GREEN = GREEN + 1;
                break;
            case ROUGE:
                RED = RED + 1;
                break;
            case NOIR:
                BLACK = BLACK + 1;
                break;
            case BLEU:
                BLUE = BLUE + 1;
                break;
            default:
                error = 1;
                break;
            }
        }
        if (error == 1)
        { // Si il y assez d'occurence, return la couleur comme valeur, sinon la boucle while recommence
        }
        else if (GREEN >= valIdentique)
        {
            // Serial.println("BACHE");
            return VERT;
        }
        else if (RED >= valIdentique)
        {
            // Serial.println("Rouge");
            return ROUGE;
        }
        else if (BLACK >= valIdentique)
        {
            // Serial.println("NOIR");
            return NOIR;
        }
        else if (BLUE >= valIdentique)
        {
            // Serial.println("Bleu");
            return BLEU;
        }

        Serial.println("Recommance la lectrue");
    }
}

int stoppingCriteria(AllStruct *allStruct)
{
    State *state = allStruct->state;
    if (state->comingBack != 1)
    {
        detecteurProximite(allStruct->state, allStruct->pin);
        if (state->detectLeft == 1 || state->detectRight == 1)
        {
            return 1;
        }
    }
    return 0;
}