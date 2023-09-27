#include <Arduino.h>
#include <LibRobus.h>

#define PIN_CH1 34
#define PIN_CH2 35
#define PIN_PWM_D 0
#define PIN_DIR_D 0
#define PIN_PWM_G 0
#define PIN_DIR_G 0


VexQuadEncoder vexR;
VexQuadEncoder vexL;
MotorControl mCtlR;
MotorControl mCtlL;

void setupVex(uint8_t _PIN_CH1,uint8_t _PIN_CH2);
void setupMotorControl(uint8_t _PIN_PWM_D,uint8_t _PIN_DIR_D, uint8_t _PIN_PWM_G,uint8_t _PIN_DIR_G);
void foward(float _speed);

void setup() {
  //setupVex(PIN_CH1,PIN_CH2);
  //setupMotorControl(PIN_PWM_D,PIN_DIR_D,PIN_PWM_G,PIN_DIR_G);
  BoardInit();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  Serial.println(ENCODER_Read(1));
}

void setupVex(uint8_t _PIN_CH1,uint8_t _PIN_CH2){
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
