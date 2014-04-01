/*
 * inc_motor.h
 *
 *  Created on: 21.11.2013
 *      Author: einball-dev
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"
#include "inc_msg.h"
#include "inc_def.h"
#include <hardware/hw_memmap.h>
#include <driverlib/gpio.h>

/*
 * Koordinaten-Offsets, für genauere Berechnugn der Fahrtwege
 * sie sind durch Variablen zu ersetzen,
 * die Über die Serielle Schnittstelle Kalibrierbar sein sollen
 */
#define Offset_X		0
#define Offset_Y		0
#define Offset_Z 		0
#define Offset_JOINT	0
#define Offset_GRIPPER 	0  	//Variationsgröße, die für Luftspalte beim greifen/loslassen sorgen soll (-> Boxsize+/-SizeVar)
#define Stepsize 		1 	//schrittbreite der Motoren
#define Halbkreis 		1 	//soll für eine 180° drehung stehen, muss aber mit der Schrittgröße vereinbart werden
#define BoxSize 		1 	//Boxengröße als Multiplikator, abhängig von Stepsize (-> GRIPPER*Boxsize*Stepsize = Greiferbreite)



//AusgabeschachtX = Home//
#define AusgabeschachtY 1
#define AusgabeschachtZ 1


// Steuerfunktionen

uint32_t Ctrl_Motor(sJob *Destination);
uint32_t Remote_Ctrl_Motor(sJob *Destination);
uint32_t RefillBox();
uint32_t EmptyBox();
uint32_t putBox(sJob *Destination);
uint32_t getBox(sJob *Destination);

#endif /* INC_MOTOR_H_ */
