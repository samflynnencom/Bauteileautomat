/*
 * motor.c
 *
 *  Created on: 21.11.2013
 *      Author: einball-dev
 */

#include "inc_motor.h"
#include "inc_dspin.h"




uint32_t getBox(sJob *Destination){

	/* Fahrtrichtung bestimmen und XY-Position Anfahren */

	if((((Destination->coordX + Offset_X) * Stepsize) - dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_X_AXIS_1, 1, (Destination->coordX + Offset_X) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_X_AXIS_1, 0, (Destination->coordX + Offset_X) * Stepsize);}

	if((((Destination->coordY + Offset_Y) * Stepsize) - dSPIN_GetParam(MOT_Y_AXIS, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_Y_AXIS, 1, (Destination->coordY + Offset_Y) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Y_AXIS, 0, (Destination->coordY + Offset_Y) * Stepsize);}


	while(1){
		if((1 & dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_STATUS)) && (1 & dSPIN_GetParam(MOT_Y_AXIS, dSPIN_STATUS))){
			// Warten, bis X und Z Position angefahren wurden
			break;
		}
	}

	/* Greifer öffnen, Z-Position anfahren, Box greifen und rausziehen */
	dSPIN_GoTo(MOT_GRIPPER, Destination->GRIPPER * (BoxSize + Offset_GRIPPER) * Stepsize);
	while(1){
		if(1 & dSPIN_GetParam(MOT_GRIPPER, dSPIN_STATUS)){
			// Warten, bis Greifer geöffnet wurde
			break;
		}
	}

	dSPIN_GoTo(MOT_Z_AXIS, Destination->coordZ * BoxSize * Stepsize);
	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position angefahren wurde
			break;
		}
	}

	dSPIN_GoTo(MOT_GRIPPER, Destination->GRIPPER * BoxSize * Stepsize);
	while(1){
		if(1 & dSPIN_GetParam(MOT_GRIPPER, dSPIN_STATUS)){
			// Warten, bis Greifer geschlossen wurde
			break;
		}
	}

	dSPIN_GoHome(MOT_Z_AXIS);
	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position zurückgesetzt wurde
			break;
		}
	}
	return EXIT_SUCCESS;
}


uint32_t putBox(sJob *Destination){


	/* Fahrtrichtung bestimmen und XY-Position Anfahren */
	if((((Destination->coordX + Offset_X) * Stepsize) - dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_ABS_POS)) >0){
		dSPIN_GoTo_DIR(MOT_X_AXIS_1, 1, (Destination->coordX + Offset_X) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_X_AXIS_1, 0, (Destination->coordX + Offset_X) * Stepsize);}

	if((((Destination->coordY + Offset_Y) * Stepsize) - dSPIN_GetParam(MOT_Y_AXIS, dSPIN_ABS_POS)) >0){
		dSPIN_GoTo_DIR(MOT_Y_AXIS, 1, (Destination->coordY + Offset_Y) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Y_AXIS, 0, (Destination->coordY + Offset_Y) * Stepsize);}


	while(1){
		if((1 & dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_STATUS)) && ( 1 &dSPIN_GetParam(MOT_Y_AXIS, dSPIN_STATUS))){
			// Warten, bis X und Y Position angefahren wurden
			break;
		}
	}


	/* Z-Position anfahren, Box ablegen und zurückfahren */

	dSPIN_GoTo(MOT_Z_AXIS, ((Destination->coordZ + Offset_Z) * Stepsize));
	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position angefahren wurde
			break;
		}
	}

	dSPIN_GoTo(MOT_GRIPPER, Destination->GRIPPER * (BoxSize + Offset_GRIPPER) * Stepsize); // Platzhalter für Greiferstellung
	while(1){
		if(1 & dSPIN_GetParam(MOT_GRIPPER, dSPIN_STATUS)){
			// Warten, bis Greifer geöffnet wurde
			break;
		}
	}

	dSPIN_GoHome(MOT_Z_AXIS);
	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position zurückgesetzt wurde
			break;
		}
	}

	return EXIT_SUCCESS;
}


uint32_t EmptyBox(){

	dSPIN_GoHome(MOT_X_AXIS_1);
	if(((AusgabeschachtY + Offset_Y) * Stepsize - dSPIN_GetParam(MOT_Y_AXIS, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_Y_AXIS, 1, (AusgabeschachtY + Offset_Y) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Y_AXIS, 0, (AusgabeschachtY + Offset_Y) * Stepsize);}

	while(1){
		// Warten, bis X und Y Position angefahren wurden
		if((1 & dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_STATUS)) && (1 & dSPIN_GetParam(MOT_Y_AXIS, dSPIN_STATUS))){
		break;
		}
	}

	dSPIN_GoTo(MOT_Z_AXIS, (AusgabeschachtZ + Offset_Z) * Stepsize);
	while(1){
		// Warten, bis  Z Position angefahren wurde
		if(1&dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
		break;
		}
	}

	dSPIN_GoTo(MOT_JOINT, Halbkreis);
	while(1){
		if(1 & dSPIN_GetParam(MOT_JOINT, dSPIN_STATUS)){
			// Warten, bis fertig Rotiert
			break;
		}
	}
	dSPIN_GoHome(MOT_JOINT);
	dSPIN_GoHome(MOT_Z_AXIS);
	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position zurückgesetzt wurde
			break;
		}
	}

	return EXIT_SUCCESS;
}

uint32_t RefillBox(){

	dSPIN_GoHome(MOT_X_AXIS_1);
	if(((AusgabeschachtY + Offset_Y) * Stepsize - dSPIN_GetParam(MOT_Y_AXIS, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_Y_AXIS, 1, (AusgabeschachtY + Offset_Y) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Y_AXIS, 0, (AusgabeschachtY + Offset_Y) * Stepsize);}

	while(1){
		if((1 & dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_STATUS)) && (1 & dSPIN_GetParam(MOT_Y_AXIS, dSPIN_STATUS))){
			// Warten, bis X und Y Position angefahren wurden
			break;
		}
	}

	dSPIN_GoTo(MOT_Z_AXIS, (AusgabeschachtZ + Offset_Z) * Stepsize);
	while(1){
		//muss noch auf den richtigen Port/Pin abgestimmt werden, siehe auch: startup.c
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
			break;
	}

	dSPIN_GoHome(MOT_Z_AXIS);

	while(1){
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
			// Warten, bis  Z Position zurückgesetzt wurde
			break;
		}

	}

	return EXIT_SUCCESS;
}

uint32_t Remote_Ctrl_Motor(sJob *Destination){



	//zum debuggen
	return EXIT_SUCCESS;



	/* X- und Y-Positionen anfahren */
	if((((Destination->coordX + Offset_X) * Stepsize) - dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_X_AXIS_1, 1, (Destination->coordX + Offset_X) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_X_AXIS_1, 0, (Destination->coordX + Offset_X) * Stepsize);}

	if((((Destination->coordY + Offset_Y) * Stepsize) - dSPIN_GetParam(MOT_Y_AXIS, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_Y_AXIS, 1,(Destination->coordY + Offset_Y) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Y_AXIS, 0, (Destination->coordY + Offset_Y) * Stepsize);}


	while(1){
		// Warten, bis X und Y Position angefahren wurden
		if((1 & dSPIN_GetParam(MOT_X_AXIS_1, dSPIN_STATUS)) && (1 & dSPIN_GetParam(MOT_Y_AXIS, dSPIN_STATUS))){
		break;
		}
	}

	/*Z-Position Anfahren ---- Die Trennung von X/Y und Z ist zum schutz des Automateninhalts gedacht, siehe auch putbox, getbox, refillbox und emptybox */
	if((((Destination->coordZ + Offset_Z) * Stepsize) - dSPIN_GetParam(MOT_Z_AXIS, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_Z_AXIS, 1,(Destination->coordZ + Offset_Z) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_Z_AXIS, 0, (Destination->coordZ + Offset_Z) * Stepsize);}


	while(1){
		// Warten, bis X und Y Position angefahren wurden
		if(1 & dSPIN_GetParam(MOT_Z_AXIS, dSPIN_STATUS)){
		break;
		}
	}

	/* Greiferbreite und Rotationsachse ändern, aus dem selben grund getrennt, wie auch bei Z-Position */
	if((((Destination->GRIPPER + Offset_GRIPPER) * Stepsize) - dSPIN_GetParam(MOT_GRIPPER, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_GRIPPER, 1, (Destination->GRIPPER + Offset_GRIPPER) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_GRIPPER, 0, (Destination->GRIPPER + Offset_GRIPPER) * Stepsize);}

	if((((Destination->JOINT + Offset_JOINT) * Stepsize) - dSPIN_GetParam(MOT_JOINT, dSPIN_ABS_POS)) > 0){
		dSPIN_GoTo_DIR(MOT_JOINT, 1, (Destination->JOINT + Offset_JOINT) * Stepsize);
	}
	else{dSPIN_GoTo_DIR(MOT_JOINT, 0, (Destination->JOINT + Offset_JOINT) * Stepsize);}


	while(1){
		if((1 & dSPIN_GetParam(MOT_GRIPPER, dSPIN_STATUS)) && ( 1 &dSPIN_GetParam(MOT_JOINT, dSPIN_STATUS))){
			// Warten, bis Greifer und Drehgelenk Verstellt wurden
			break;
		}
	}

	return EXIT_SUCCESS;
}

uint32_t Ctrl_Motor(sJob *Destination){

	sJob sort;
	sJob FREE_SPACE;

	//zum debuggen
	return EXIT_SUCCESS;


	/*
	 * FREE_SPACE ist das Sortierfeld, in das Kisten umgelagert werden,
	 * um an Hintere Kisten zu kommen, z.B.:
	 * man will an Kiste Nr.3 (coordZ=3), also lagert man Kisten 1 und 2 in FREE_SPACE,
	 * bevor man sich Kiste 3 holt
	 * Die switch anweisung überprüft, welches Sortierfeld gebraucht wird
	 */
	FREE_SPACE.JOINT=FREE_SPACE_JOINT;
	switch (Destination->GRIPPER){
	case(1):
		FREE_SPACE.coordX=FREE_SPACE_X_1;
		FREE_SPACE.coordY=FREE_SPACE_Y_1;
		FREE_SPACE.coordZ=FREE_SPACE_Z_1;
		FREE_SPACE.GRIPPER=Destination->GRIPPER;
		break;
	case(2):
		FREE_SPACE.coordX=FREE_SPACE_X_2;
		FREE_SPACE.coordY=FREE_SPACE_Y_2;
		FREE_SPACE.coordZ=FREE_SPACE_Z_2;
		FREE_SPACE.GRIPPER=Destination->GRIPPER;
		break;
	// Hier mehr Größen einfügen, nach bedarf
	}

	sort=*Destination;
	sort.coordZ--;
	/*
	 * Hier Greiferstellung Anpassen
	 */
	switch(Destination->opCode){
	case OPC_AUSGABE :
		if(Destination->coordZ>1){ 		//Wenn Schachtel nicht in erster Reihe steht
			getBox(&sort);				//getBox fährt an die richtige Position und holt die angegebene Box
			//analog dazu: putBox stellt die Box in Frage an die angegebene Position
			putBox(&FREE_SPACE);		//FREE_SPACE_N ist ein Leerer Schacht der Boxgröße N
		}								//Die Boxen werden immer so weit hinten wie möglich platziert
		getBox(Destination);
		EmptyBox();						//EmptyBox fährt zum ausgabeschacht und rotiert den A-Motor
		putBox(Destination);

		if(Destination->coordZ>1){				//die Umsortierten Schachteln werden zurückgestellt
			getBox(&FREE_SPACE);
			putBox(&sort);
		}
		break;
	case OPC_NACHFUELLEN:
		if(Destination->coordZ>1){
			getBox(&sort);
			putBox(&FREE_SPACE);
		}
		getBox(Destination);
		RefillBox();
		putBox(Destination);

		if(Destination->coordZ>1){
			getBox(&FREE_SPACE);
			putBox(&sort);
		}
		break;
	}
	////return EXIT_FAIL muss auch noch irgendwie festgelegt werden
	////-> Error states in den o.g. Funktionen setzen
	return EXIT_SUCCESS;
}


