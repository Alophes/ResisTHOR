#include <Arduino.h>
#include <LibRobus.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

void foward(float _speed);
void stopMotors();
void motorsAccelerate(float _speedWanted);
void motorsDeccelerate(float ActualSpeed);
void checkPulse();

struct Pulse {
  uint16_t right;
  uint16_t left;
  uint16_t rightCnt = 0;
  uint16_t leftCnt = 0;
  uint16_t rightPast;
  uint16_t leftPast;
};

struct Pulse pulse;
float speed;
bool doOnce = true;
void (*foo)();
foo = &checkPulse;
void setup() {
  BoardInit();
  Serial.begin(9600);
  speed=0.75;
  motorsAccelerate(speed);
  SOFT_TIMER_SetCallback(0, (*foo)());
  SOFT_TIMER_SetDelay(0,10);
  SOFT_TIMER_SetRepetition(0,-1);
  SOFT_TIMER_Enable(0);
}

void loop() {
  SOFT_TIMER_Update();
  if (pulse.left >= 3200) {
    ENCODER_Reset(ENCODER_LEFT);
    pulse.leftCnt++;
  }
  if (pulse.right >= 3200) {
    ENCODER_Reset(ENCODER_RIGHT);
    pulse.rightCnt++;
  }
  if(pulse.rightCnt >= 18 || pulse.leftCnt >= 18) {
    if(doOnce == true){
      doOnce = false;
      motorsDeccelerate(speed);
    }
    else
      stopMotors();
  }
  else
    foward(speed);

}
void checkPulse(){
  pulse.leftPast=pulse.left;
  pulse.rightPast=pulse.right;
  pulse.left = ENCODER_Read(ENCODER_LEFT);
  pulse.right = ENCODER_Read(ENCODER_RIGHT);
}
void motorsDeccelerate(float ActualSpeed){
  uint8_t delayMs = 75;
  foward(ActualSpeed*0.90);
  delay(delayMs);
  foward(ActualSpeed*0.80);
  delay(delayMs);
  foward(ActualSpeed*0.70);
  delay(delayMs);
  foward(ActualSpeed*0.60);
  delay(delayMs);
  foward(ActualSpeed*0.50);
  delay(delayMs);
  foward(ActualSpeed*0.40);
  delay(delayMs);
  foward(ActualSpeed*0.30);
  delay(delayMs);
  foward(ActualSpeed*0.20);
  delay(delayMs);
  foward(ActualSpeed*0.10);
  delay(delayMs);
  stopMotors();
}
void motorsAccelerate(float _speedWanted){
  uint8_t delayMs = 150;
  foward(_speedWanted*0.10);
  delay(delayMs);
  foward(_speedWanted*0.20);
  delay(delayMs);
  foward(_speedWanted*0.30);
  delay(delayMs);
  foward(_speedWanted*0.40);
  delay(delayMs);
  foward(_speedWanted*0.50);
  delay(delayMs);
  foward(_speedWanted*0.60);
  delay(delayMs);
  foward(_speedWanted*0.70);
  delay(delayMs);
  foward(_speedWanted*0.80);
  delay(delayMs);
  foward(_speedWanted*0.90);
  delay(delayMs);
  foward(_speedWanted);
}
void foward(float _speed){
  if(pulse.right < pulse.left) {
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed+0.005);
    //MOTOR_SetSpeed(MOTOR_LEFT,_speed-0.05);
  }
  if(pulse.right > pulse.left) {
    MOTOR_SetSpeed(MOTOR_LEFT,_speed+0.005);
    //MOTOR_SetSpeed(MOTOR_RIGHT,_speed-0.05);
  }
  if(pulse.right == pulse.left) {
    MOTOR_SetSpeed(MOTOR_LEFT,_speed);
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed);
  }
  if(pulse.leftPast != pulse.left) {
  Serial.print("LEFT ");
  Serial.println(pulse.left);
  }
  if(pulse.rightPast != pulse.right) {
  Serial.print("RIGHT ");
  Serial.println(pulse.right);
  }
}

void stopMotors(){
 MOTOR_SetSpeed(MOTOR_LEFT,0);
 MOTOR_SetSpeed(MOTOR_RIGHT,0);
}
