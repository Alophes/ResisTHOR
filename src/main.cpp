#include <Arduino.h>
#include <LibRobus.h>

#define ENCODER_LEFT 0
#define ENCODER_RIGHT 1

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

#define mod1 42
#define led_mod1 43

#define mod2 44
#define led_mod2 45

void foward(float _speed);
void stopMotors();
void motorsAccelerate(float _speedWanted);

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
void setup() {
  BoardInit();
  Serial.begin(9600);
  speed=1;
  motorsAccelerate(speed);

  //detecteur de proximité
  pinMode(42, INPUT); //mod1
  pinMode(44, INPUT); //mod2
  pinMode(43, OUTPUT); //ledmod1
  pinMode(45, OUTPUT); //ledmod2
  
  
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
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed+0.15);
    MOTOR_SetSpeed(MOTOR_LEFT,_speed-0.15);
  }
  if(pulse.right > pulse.left) {
    MOTOR_SetSpeed(MOTOR_LEFT,_speed+0.15);
    MOTOR_SetSpeed(MOTOR_RIGHT,_speed-0.15);
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

void detecteurProximite(){

  
  while(1){
    
    if(ROBUS_IsBumper(FRONT)==HIGH){
      break;
    }

    if(digitalRead(mod1)==HIGH && digitalRead(mod2)==LOW){
      digitalWrite(led_mod1, HIGH);
    }

    if(digitalRead(mod1)==LOW && digitalRead(mod2)==HIGH){
      digitalWrite(led_mod2, HIGH);
    }

    if(digitalRead(mod1)==HIGH && digitalRead(mod2)==HIGH){
      digitalWrite(led_mod1, HIGH);
      digitalWrite(led_mod2, HIGH);
    }

    else{
      digitalWrite(led_mod1, HIGH);
      digitalWrite(led_mod2, HIGH);
    }

    delay(100);
  }
}

void loop() {
  
  if(ROBUS_IsBumper(FRONT)==HIGH){
    detecteurProximite();
  }
  
}