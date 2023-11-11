#include "util.h"
#include <stdio.h>


Speed *initSpeed(){
	Speed *vitesse = (Speed*)malloc(sizeof(Speed));
	
	if(baseSet.robot == 'A'){
		vitesse->forwardLeft = baseSet.speedRobotA.forwardL;
		vitesse->forwardRight = baseSet.speedRobotA.forwardR;
		vitesse->accelerationLeft = baseSet.speedRobotA.accelerationL;
		vitesse->accelerationRight = baseSet.speedRobotA.accelerationR;
		vitesse->decelerationLeft = baseSet.speedRobotA.decelerationL;
		vitesse->decelerationRight = baseSet.speedRobotA.decelerationR;
	}
	
	if(baseSet.robot == 'B'){
		vitesse->forwardLeft = baseSet.speedRobotB.forwardL;
		vitesse->forwardRight = baseSet.speedRobotB.forwardR;
		vitesse->accelerationLeft = baseSet.speedRobotB.accelerationL;
		vitesse->accelerationRight = baseSet.speedRobotB.accelerationR;
		vitesse->decelerationLeft = baseSet.speedRobotB.decelerationL;
		vitesse->decelerationRight = baseSet.speedRobotB.decelerationR;
	}
	return vitesse;
}

State *initState(){

  State *etat = (State*)malloc(sizeof(State));

  etat->coordinates[0] = 0;
  etat->coordinates[1] = 0;

  etat->detectLeft = 0;
  etat->detectRight = 0;

  etat->moving = 0;

  etat->begin = 1;

  return etat;
}

Pulse *initPulse(){

  Pulse * pul = (Pulse*)malloc(sizeof(Pulse));

  pul->left = 0;
  pul->right = 0;
  
  return pul;

}

