/*
 * msg.c
 *
 *  Created on: 19.10.2013
 *      Author: einball
 */

#include "inc_msg.h"

#include "inc_circular.h"


/* addJob fügt einen Job zur Warteschlange hinzu */
uint32_t addJob(sJobQueue *Queue, sJob *Job){

	uint32_t i;

	for(i = 0; i < 10; i++){
		if(Queue->Job[i].id == 0){
			Queue->Job[i] = *Job;
			return EXIT_SUCCESS;
		}
	}
	//wird kein Feld mit leerer ID gefunden so ist die Warteschlange voll: EXIT_FAILURE
	return EXIT_FAILURE;
}


/* callJob überprüft zuerst ob der aktuelle Job leer ist, wenn nicht muss dieser erst bearbeitet werden (return JOB_PENDING).
 * Ist der Job leer wird das erste Element der Warteschlange, falls vorhanden, in den Job geschrieben und die restlichen Elemente werden eins aufgerückt.
 * Ist die Warteschlange leer wird ein EXIT_FAILURE zurückgegeben
 */
uint32_t callJob(sJobQueue *Queue, sJob *Job){
	uint32_t i;

	if(Job->id != 0)
		/* es wartet noch ein Job auf bearbeitung -> bearbeitung einleiten */
		return JOB_PENDING;

	/* Abfrage, ob Element 0 in der Warteschlange ist */
	if(Queue->Job[0].id != 0){
		/* Erstes Element der Warteschlange In den Motorsteuerungs-Puffer laden */
		Job->coordX = Queue->Job[0].coordX;
		Job->coordY = Queue->Job[0].coordY;
		Job->coordZ = Queue->Job[0].coordZ;
		Job->JOINT = Queue->Job[0].JOINT;
		Job->GRIPPER = Queue->Job[0].GRIPPER;
		Job->crc = Queue->Job[0].crc;
		Job->opt1 = Queue->Job[0].opt1;
		Job->opt2 = Queue->Job[0].opt2;
		Job->opt3 = Queue->Job[0].opt3;

		/* Alle Elemente der Warteschlange eins "Vorrücken" und die letzten elemente zu Null setzen */
		for(i = 0; i < 10; i++){
			if(Queue->Job[i+1].id == 0){
				Queue->Job[i].id = 0;
				Queue->Job[i].opCode = 0;
				Queue->Job[i].coordX = 0;
				Queue->Job[i].coordY = 0;
				Queue->Job[i].coordZ = 0;
				Queue->Job[i].JOINT = 0;
				Queue->Job[i].GRIPPER = 0;
				Queue->Job[i].crc = 0;
				Queue->Job[i].opt1 = 0;
				Queue->Job[i].opt2 = 0;
				Queue->Job[i].opt3 = 0;
			}
			else Queue->Job[i] = Queue->Job[i+1];
		}
		return EXIT_SUCCESS;
	}
	else return EXIT_FAILURE;
}


/* jobDone leert den übergebenen Job, was normalerweise der letzte bearbeitete Job ist */
uint32_t jobDone(sJob *Job){

	Job->id = 0;
	Job->opCode = 0;
	Job->coordX = 0;
	Job->coordY = 0;
	Job->coordZ = 0;
	Job->JOINT = 0;
	Job->GRIPPER = 0;
	Job->crc = 0;
	Job->opt1 = 0;
	Job->opt2 = 0;
	Job->opt3 = 0;

	return EXIT_SUCCESS;
}

/* checkCRC soll, wie der Name veruten lässt, einen CR-Check durchführen. Dieses Feature ist allerdings noch nicht Implementiert. */
uint32_t CheckCRC(uint32_t CRC){

	return EXIT_SUCCESS;
}


/* validateID überprüft die empfangene ID auf gültigkeit.
 * Ähnlich wie bei der CRC prüfung ist diese Funktion noch nicht vollständig Implementiert. Derzeit geht die Funktion lediglich die Warteschlange
 * durch und überprüft die IDs der anderen Jobs auf übereinstimmung, um eine "Doppelbelegung" zu vermeiden.
 */
uint32_t validateID(sJobQueue *Queue, uint32_t ID){
	uint32_t i;
	for(i=0;i<10;i++){
		if(Queue->Job[i].id == ID){
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}


/* Im Gegensatz zu validateID und chekcCRC dient validateMsg keinen Formellen (Schutz vor Missbrauch, bzw Betrug), sondern Praktischen Zwecken:
 * Die Funktion überprüft ob die empfangenen Parameter Sinn ergeben, um Schäden am Automaten vorzubeugen.
 * Die Überprüfung von opt1-3 ist zwar vorgesehen aber noch nicht Implementiert, da opt1-3 noch nicht mit Inhalt belegt sind.
 */
uint32_t validateMsg(sJob *Job){

	/* eventuelle weitere opCodes müssen natürlcih auch noch hier eingefügt werden,
	 * da die funktion sonst automatisch in den default-case geht */
	switch(Job->opCode){
	case OPC_AUSGABE:
		/* alle Koordinaten müssen größer als null sein, sonst liegt ein
		 * Fehlerfall vor, da (0,0,0) die Home Position ist und die Kästchenbreite
		 * immer >=1 ist...nur die Rotationsachse darf ausschließlich 0 sein,
		 * da sonst das aufnehmen der boxen nicht klappt */
		if((Job->coordX == 0) || (Job->coordY == 0) || (Job->coordZ == 0) || (Job->GRIPPER == 0) || (Job->JOINT != 0))
			return EXIT_FAILURE;

		/* Genauso, wie ein Parameter immer größer 0 sein muss, darf natürlich ein Maximum nicht überschritten werden */
		if((Job->coordX > Max_Pos_X) || (Job->coordY > Max_Pos_Y) || (Job->coordZ > Max_Pos_Z) || (Job->GRIPPER > Max_Pos_GRIPPER))
			return EXIT_FAILURE;

		/*opt1..opt3 ???*/
		break;

	case OPC_NACHFUELLEN:
		/* beim nachfüllen gelten vorerst die selben bedingungen wie beim Ausgeben, sie sind allerdings getrennt aufgeführt,
		 * falls später eine Trennung erforderlich wird */
		if((Job->coordX <= 0) || (Job->coordY <= 0) || (Job->coordZ <= 0) || (Job->GRIPPER <= 0) || (Job->JOINT != 0))
			return EXIT_FAILURE;

		if((Job->coordX > Max_Pos_X) || (Job->coordY > Max_Pos_Y) || (Job->coordZ > Max_Pos_Z) || (Job->GRIPPER > Max_Pos_GRIPPER))
			return EXIT_FAILURE;

		/*opt1..opt3 ???*/
		break;

	case OPC_FERNSTEUERN:
		/* auch hier gilt: grenzen müssen eingehalten werden! weitere bedingungen fehlen auch hier */
		if((Job->coordX <= 0) || (Job->coordY <= 0) || (Job->coordZ <= 0) || (Job->GRIPPER <= 0) || (Job->JOINT != 0))
			return EXIT_FAILURE;

		if((Job->coordX > Max_Pos_X) || (Job->coordY > Max_Pos_Y) || (Job->coordZ > Max_Pos_Z) || (Job->GRIPPER > Max_Pos_GRIPPER))
			return EXIT_FAILURE;
		break;

	case OPC_REKALIBRIEREN:
		break;

	default:
		/* wenn wir in den default-case fallen stimmt irgendwas mit dem opCode nicht
		 * -> Abbruch und Job leeren */
		return EXIT_FAILURE;
	}
	/* kein fehler ist aufgetreten: die Empfangene Nachricht ist Valide und der Job kann weitergeführt werden */
	return EXIT_SUCCESS;
}


/* decodeMsg entnimmt den empfangenen Datenstrom aus dem Ringpuffer und trennt die einzelnen Bestandteile,
 * überprüft und Speichert diese als Parameter in einem Job ab, sofern alles in Ordnung ist.
 * Dieser Job wird als JobBuffer in der Statemachine weiterverarbeitet.
 */
uint32_t decodeMsg(sJobQueue *Queue, sJob *Buf, uint8_t *MsgPointer){
	uint32_t i=0, iSection = 1;
	uint8_t *ID_Buf, *CRC_Buf, *OPCode_Buf, *X_Buf, *Y_Buf, *Z_Buf, *JOINT_Buf, *GRIPPER_Buf;
	/*  uint32_t optArgA, optArgB, optArgC;
		uint32_t *optArgA_Buf, *optArgB_Buf, *optArgC_Buf;
	 */


	/* ID_Buf hält die ID des Jobs. Die ID muss als erstes stehen */
	ID_Buf = MsgPointer;

	while( *(MsgPointer+i) != '[' ){
		i++;
		if(*(MsgPointer+i) == '\n'){
			return MSG_CODE_ERROR;
		}
	}

	/* Der Pointer + i zeigt nun auf die öffnende Klammer.
	 * Die wird kurzerhand mit der Stringende Kennung ersetzt.
	 */
	*(MsgPointer+i) = '\0';
	i++;

	/* Nun muss zwingend eine Kommata separierte Liste folgen!
	 * Was aber auf jeden Fall klar ist, ist dass nun der OpCode folgt!
	 */
	OPCode_Buf = (MsgPointer + i);

	while( *(MsgPointer + i) != ']' ){

		i++;

		if(*(MsgPointer+i) == '\n'){
			return MSG_CODE_ERROR;
		}

		if( *(MsgPointer + i) == ',' ){

			switch(iSection){
			case 1:
				*(MsgPointer + i) = '\0';
				iSection = 2;
				X_Buf = (MsgPointer + i + 1);
				break;

			case 2:
				*(MsgPointer + i) = '\0';
				iSection = 3;
				Y_Buf = (MsgPointer + i + 1);
				break;

			case 3:
				*(MsgPointer + i) = '\0';
				iSection = 4;
				Z_Buf = (MsgPointer + i + 1);
				break;

			case 4:
				*(MsgPointer + i) = '\0';
				iSection = 5;
				JOINT_Buf = (MsgPointer + i + 1);
				break;

			case 5:
				*(MsgPointer + i) = '\0';
				iSection = 6;
				GRIPPER_Buf = (MsgPointer + i + 1);
				break;

			case 6:
				*(MsgPointer + i) = '\0';
				iSection++;
				//optArgA_Buf = MsgPointer + i + 1;
				break;

			case 7:
				*(MsgPointer + i) = '\0';
				iSection++;
				//optArgB_Buf = MsgPointer + i + 1;
				break;

			case 8:
				*(MsgPointer + i) = '\0';
				iSection++;
				//optArgC_Buf = MsgPointer + i + 1;
				break;

			case 9:
				*(MsgPointer + i) = '\0';
				iSection++;
				break;

			default:
				/* Wir tun hier einfach nichts, denn
				 * alle weiteren Argumente sind uns egal:
				 * Wir haben erstmal nur 3!
				 */
				break;
			}
		}

	}

	*(MsgPointer + i) = '\0';

	/* Nun sind wir beim CRC Code angelangt */
	CRC_Buf = (MsgPointer + i + 1);
	while( *(MsgPointer + i) != '\n'){
		i++;
	}

	*(MsgPointer + i) = '\0';


	/* Nun können wir uns ans decoden mit atoi() usw machen */


	Buf->id=atoi((char*)ID_Buf);
	Buf->crc=atoi((char*)CRC_Buf);

	/* Job NUR speichern, wenn ID und CR-Check erfolgreich */
	if(validateID(Queue, Buf->id)==EXIT_SUCCESS){
		if(CheckCRC(Buf->crc)==EXIT_SUCCESS){
			Buf->opCode=atoi((char*)OPCode_Buf);	//String in uint32 konvertieren
			Buf->coordX=atoi((char*)X_Buf);
			Buf->coordY=atoi((char*)Y_Buf);
			Buf->coordZ=atoi((char*)Z_Buf);
			Buf->JOINT=atoi((char*)JOINT_Buf);
			Buf->GRIPPER=atoi((char*)GRIPPER_Buf);
			// Hier einfügen:
			// optArgA
			// optArgB
			// optArgC
			//
		}
		else{
			return MSG_CRC_ERROR;
		}
		if(validateMsg(Buf) == EXIT_FAILURE){
			/* irgendetwas stimmt nicht mit der Nachricht: Löschen und Fehler ausgeben */
			jobDone(Buf);
			return MSG_INVALID;
		}

	}
	else{
		return MSG_ID_ERROR;
	}
	return MSG_ALL_OK;
}
