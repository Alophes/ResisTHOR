#include <Arduino.h>
#include <librobus.h>

#define PIN_CH1_R 
#define PIN_CH2_R
#define PIN_CH1_L
#define PIN_CH2_L 
#define PIN_PWM_D 0
#define PIN_DIR_D 0
#define PIN_PWM_G 0
#define PIN_DIR_G 0


VexQuadEncoder vexR;
VexQuadEncoder vexL;
MotorControl mCtlR;
MotorControl mCtlL;

void setupVexR(uint8_t _PIN_CH1,uint8_t _PIN_CH2);
void setupMotorControl(uint8_t _PIN_PWM_D,uint8_t _PIN_DIR_D, uint8_t _PIN_PWM_G,uint8_t _PIN_DIR_G);
void foward(float _speed);



void setup() {
  setupVexR(PIN_CH1_R,PIN_CH2_R);
  setupMotorControl(PIN_PWM_D,PIN_DIR_D,PIN_PWM_G,PIN_DIR_G);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void setupVexR(uint8_t _PIN_CH1,uint8_t _PIN_CH2){
  vexR.init(_PIN_CH1,_PIN_CH2);
  attachInterrupt(vexR.getPinInt(), []{vexR.isr();}, FALLING);
}
void setupMotorControl(uint8_t _PIN_PWM_D,uint8_t _PIN_DIR_D, uint8_t _PIN_PWM_G,uint8_t _PIN_DIR_G){
  mCtlR.init(_PIN_PWM_D,_PIN_DIR_D);
  mCtlL.init(_PIN_PWM_G,_PIN_DIR_G);
}

void foward(float _speed, float distance){

  mCtlR.setSpeed(_speed);
  mCtlL.setSpeed((0-_speed));
}

void turnLeftOnItSelf(float _speed, uint16_t _angle){
  mCtlR.setSpeed(_speed);
  mCtlL.setSpeed(_speed);
}
