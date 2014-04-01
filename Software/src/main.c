/*
 * main.c
 *
 *  Created on: 19.10.2013
 *      Author: einball
 */
#if 1 // ausblenden der #include befehle, zum verbessern des "workflows"
#define TARGET_IS_BLIZZARD_RA2
#define TM4C123GH6PM
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/ssi.h>
#include <driverlib/gpio.h>
#include <driverlib/watchdog.h>
#include <driverlib/timer.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

#include <hardware/hw_nvic.h>
#include <hardware/hw_ints.h>
#include <hardware/hw_sysctl.h>
#include <hardware/hw_uart.h>
#include <hardware/hw_ssi.h>
#include <hardware/hw_gpio.h>
#include <hardware/hw_watchdog.h>
#include <hardware/hw_timer.h>
#include <hardware/hw_memmap.h>

#include "main.h"
#include "inc_def.h"		// Our own defines
#include "inc_startup.h"	// Startup Funkctions
#include "inc_uart.h"		// UART Handler functions
#include "inc_dspin.h"		// SPI Handler functions
#include "inc_lcd.h"		// LCD handler functions
#include "inc_msg.h"		// Protocol support
#include "inc_circular.h"	// Circular Buffer
#include "inc_motor.h"		// Motor handler functions

#endif

/* Global definierte Ringpuffer, da wir sie sowohl in der Main als auch im ISR-Handler brauchen (keine Übergabewerte */
Circ_Buf Ringpuffer_RX;
Circ_Buf Ringpuffer_TX;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////    UART ISR	//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void UARTIntHandler(void){
	uint32_t ui32Status;
	uint8_t  c;

	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);

	if((ui32Status & UART_INT_TX) || (eState.SER & SER_TX_REQUEST)){    	// auf Transmit Interrupt abfragen

		c = getChar(&Ringpuffer_TX);
		while(c != EXIT_BUFFER_EMPTY){			//prüfen auf fehlerbedingunng
			UARTCharPut(UART0_BASE, c);			//Zeichen in FIFO schieben
			c = getChar(&Ringpuffer_TX);
		}
		ui32Status ^= UART_INT_TX;
		eState.SER ^= SER_TX_REQUEST;
		GOTO_STATE(STATE_IDLE);
		//Finished with transmission
	}

	if((ui32Status & UART_INT_RX) || (ui32Status & UART_INT_RT)){

		while(UARTCharsAvail(UART0_BASE)){
			c = UARTCharGet(UART0_BASE);
			if(putChar(&Ringpuffer_RX, c) == EXIT_BUFFER_FULL){

				//We have had some kind of undetected buffer overflow
				eState.SER |= SER_RX_BUFFER_OVERRUN;
				GOTO_STATE(STATE_ERROR);

				//In the error state we will have to scan for remaining messages
				//and flush the buffer then!

			}

			if(c == '\n'){
				//We have a valid message! Do something about it!
				eState.SER |= SER_RX_MESSAGE_COMPLETE;
				GOTO_STATE(STATE_UART_COM);
			}
		}
		/* Empfang Vollständig - softwareflags Löschen */
		ui32Status &= !UART_INT_RX;
		ui32Status &= !UART_INT_RT;
	}

	if(ui32Status &  UART_INT_OE){
		ui32Status ^= UART_INT_OE;
		eState.SER |= SER_RX_ERROR;
	}

	if(ui32Status & UART_INT_BE){
		ui32Status ^= UART_INT_BE;
		eState.SER |= SER_RX_ERROR;
	}

	if(ui32Status & UART_INT_PE){
		ui32Status ^= UART_INT_PE;
		eState.SER |= SER_RX_ERROR;
	}

	if(ui32Status & UART_INT_FE){
		ui32Status ^= UART_INT_FE;
		eState.SER |= SER_RX_ERROR;
	}

	if(ui32Status != 0){
		GOTO_STATE(STATE_ERROR);
	}
}


/* Init_Transmission Startet den Sendevorgang über die UART-Schnittstelle
 * schnelle Dörte Lösung, aber es funktioniert
 */
void Init_Transmission(Circ_Buf *Buf){
	uint8_t c;
	c = getChar(Buf);
	if(c != EXIT_BUFFER_EMPTY)
		UARTCharPut(UART0_BASE, c);

	/* das programm läuft einmal "zu oft" in Init_Transmission - das nutzen wir hier aus, um den Idle-State aufzurufen */
	else GOTO_STATE(STATE_IDLE);
}


/* insertString dient als Wrapper-Funktion um einen String in send[128] einzufügen
 * die if-abfrage zu Beginn überprüft, ab wo platz zum schreiben ist
 */
uint32_t insertString(uint8_t str[128], uint8_t buf[128]){
	uint8_t i = 0;
	uint8_t ii = 0;

	while(str[ii] != 0)
		ii++;


	for(i = 0; i < 128; i++){
		*(str + ii + i) = buf[i];
	}
	return EXIT_SUCCESS;
}


/* clearString ist eine Wrapper-Funktion dessen zweck das löschen von send[128] ist */
uint32_t clearString(uint8_t str[128]){
	uint8_t i = 0;
	for(i = 0; i < 128; i++){
		str[i] = 0;
	}
	return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////    Main	//////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void) {
	uint32_t i;
	uint32_t ii=0;
	sJob JobBuffer;
	sJob CurrentJob;
	uint8_t MsgPuffer[128];
	uint8_t send [128];
	uint16_t BufSize = 256;
	sJobQueue Job_Queue;
	uint32_t retVal;

	GOTO_STATE(STATE_STARTUP);

	while(1){

		////////////////// 	State Machine	////////////////////
		switch(SYS_STATE) {

		case STATE_STARTUP:
			SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
			setup_GPIO();
			setup_UART();
			setup_SPI();
			setup_INT();
			setup_HOME_POS();
			setup_JOB_QUEUE(&Job_Queue);

			jobDone(&CurrentJob);
			jobDone(&JobBuffer);

			clearString(send);
			clearString(MsgPuffer);

			Ringpuffer_RX.rdptr = BUF_SIZE - 1;
			Ringpuffer_RX.wrptr = 0;
			Ringpuffer_RX.size = BUF_SIZE;

			Ringpuffer_TX.rdptr = BUF_SIZE - 1;
			Ringpuffer_TX.wrptr = 0;
			Ringpuffer_TX.size = BUF_SIZE;

			GOTO_STATE(STATE_IDLE);
			break;


		case STATE_ERROR:

			switch (eState.SER){
			case SER_RX_TIMEOUT:

				insertString(send, (uint8_t*)" SER_RX_TIMEOUT ");
				insertString(send, (uint8_t*)" NACK ");
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_RX_TIMEOUT;
				clearString(send);
				for(i=0; i < 125; i++)
					*(send+i) = 0;
				break;

			case SER_RX_IDLE:
				insertString(send, (uint8_t*)" SER_RX_IDLE ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_RX_IDLE;
				clearString(send);
				break;

			case SER_RX_ERROR:
				/* Dekodieren fehlgeschlagen, NACK senden */
				insertString(send, (uint8_t*)" SER_RX_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_RX_ERROR;
				clearString(send);
				break;

			case SER_TX_ERROR:
				insertString(send, (uint8_t*)" SER_TX_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_TX_ERROR;
				clearString(send);
				break;

			case SER_RX_BUFFER_OVERRUN:
				insertString(send, (uint8_t*)" SER_RX_BUFFER_OVERRUN ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_RX_BUFFER_OVERRUN;
				clearString(send);
				break;

			case SER_RX_BUFFER_EMPTY:
				insertString(send, (uint8_t*)" SER_RX_BUFFER_EMPTY ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.SER ^= SER_RX_BUFFER_EMPTY;
				clearString(send);
				break;

			default:
				break;
			}

			switch (eState.STA) {
			default:
				break;
			}

			switch (eState.ETH){
			default:
				break;
			}

			switch (eState.MSG){
			case MSG_LENGTH_ERROR:
				// Nachricht zu lang, bzw. Puffer voll
				insertString(send, (uint8_t*)" MSG_LENGTH_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_LENGTH_ERROR;
				clearString(send);
				break;

			case MSG_INVALID:
				// Nachriten-inhalt fehlerhaft
				insertString(send, (uint8_t*)" MSG_INVALID ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_INVALID;
				clearString(send);
				break;

			case MSG_ID_ERROR:
				// Nachriten-ID fehlerhaft
				insertString(send, (uint8_t*)" MSG_ID_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_ID_ERROR;
				clearString(send);
				break;

			case MSG_CRC_ERROR:
				// CRC-Fehler
				insertString(send, (uint8_t*)" MSG_ID_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_CRC_ERROR;
				clearString(send);
				break;

			case MSG_CODE_ERROR:
				// Fehler im Coderahmen
				insertString(send, (uint8_t*)" MSG_CODE_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_CODE_ERROR;
				clearString(send);
				break;

			case MSG_UNKNOWN_ERROR:
				// Unbekannter Fehler
				insertString(send, (uint8_t*)" MSG_UNKNOWN_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				eState.MSG ^= MSG_UNKNOWN_ERROR;
				clearString(send);
				break;

			default:
				break;
			}

			switch (eState.MOT){
			default:

				break;
			}

			switch (eState.JOB){
			case JOB_ADD_ERROR:

				insertString(send, (uint8_t*)" JOB_ADD_ERROR ");
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.JOB ^= JOB_ADD_ERROR;
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				clearString(send);
				break;

			case JOB_UNKNOWN_ERROR:
				insertString(send, (uint8_t*)" JOB_UNKNOWN_ERROR ");
				insertString(send, (uint8_t*)JobBuffer.id);
				insertString(send, (uint8_t*)" NACK ");
				putMsg(&Ringpuffer_TX, send);
				eState.JOB ^= JOB_UNKNOWN_ERROR;
				eState.SER |= SER_TX_REQUEST;
				GOTO_STATE(STATE_UART_COM);
				jobDone(&JobBuffer);
				clearString(send);
				break;

			default:
				break;
			}
			break;

			case STATE_JOB_HANDLING:

				if(eState.JOB & JOB_ADD_PENDING){
					/* Job zur Warteschlange hinzufügen und auf Erfolg Überprüfen,
					 * und Abarbeitung bzw. Fehlerbehandlung einleiten
					 */
					retVal = addJob(&Job_Queue, &JobBuffer);

					if(retVal == EXIT_SUCCESS){
						insertString(send, (uint8_t*)" JOB_ADD_SUCCESS ");
						insertString(send, (uint8_t*)JobBuffer.id);
						putMsg(&Ringpuffer_TX, send);
						clearString(send);
						jobDone(&JobBuffer);
						eState.SER |= SER_TX_REQUEST;
						GOTO_STATE(STATE_UART_COM);
					}
					else{
						eState.JOB |= JOB_ADD_ERROR;
						GOTO_STATE(STATE_ERROR);
					}
					eState.JOB ^= JOB_ADD_PENDING;
				}
				else GOTO_STATE(STATE_IDLE);

				break;

			case STATE_MSG_HANDLING:

				if(eState.SER & SER_RX_DECODE_VALID_MSG){
					retVal = decodeMsg(&Job_Queue, &JobBuffer, MsgPuffer);

					clearString(MsgPuffer);
					eState.SER ^= SER_RX_DECODE_VALID_MSG;

					switch (retVal){
					case MSG_ALL_OK:
						eState.JOB |= JOB_ADD_PENDING;
						GOTO_STATE(STATE_JOB_HANDLING);
						break;

					case MSG_INVALID:
						eState.MSG |= MSG_INVALID;
						GOTO_STATE(STATE_ERROR);
						break;

					case MSG_CRC_ERROR:
						eState.MSG |= MSG_CRC_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;

					case MSG_ID_ERROR:
						eState.MSG |= MSG_ID_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;

					case MSG_CODE_ERROR:
						eState.MSG |= MSG_CODE_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;

					default:
						eState.MSG |= MSG_UNKNOWN_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;
					}
				}
				else GOTO_STATE(STATE_IDLE);
				retVal=0;
				break;


			case STATE_UART_COM:

				if(eState.SER & SER_RX_MESSAGE_COMPLETE){
					retVal = getMsg(&Ringpuffer_RX, MsgPuffer, BufSize);

					switch(retVal){
					case EXIT_SUCCESS:
						GOTO_STATE(STATE_MSG_HANDLING);
						eState.SER |= SER_RX_DECODE_VALID_MSG;
						break;

					case EXIT_BUFFER_SIZE:
						eState.SER |= MSG_LENGTH_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;

					case EXIT_BUFFER_EMPTY:
						eState.SER |= SER_RX_BUFFER_EMPTY;
						GOTO_STATE(STATE_ERROR);
						break;

					default:
						eState.SER |= SER_UNDEFINED_ERROR;
						GOTO_STATE(STATE_ERROR);
						break;
					}
					eState.SER ^= SER_RX_MESSAGE_COMPLETE;	// Flag Löschen
				}

				if(eState.SER & SER_TX_REQUEST){
					//Init_Transmission startet das Senden der UART schnittstelle
					Init_Transmission(&Ringpuffer_TX);
				}
				retVal = 0;
				break;


			case STATE_MOT_COM:

				/* Je Nach opCode wird eine entsprechende aktion ausgeführt */
				switch (CurrentJob.opCode){

				case (OPC_AUSGABE || OPC_NACHFUELLEN):
					/* Ein- und Ausgabe - "Normalbetrieb" */
					Ctrl_Motor(&CurrentJob);
					break;


				case OPC_FERNSTEUERN:
					/* Für "Manuelle" Steuerung der Motoren:
					 * "DEMO"/Test -Modus */
					Remote_Ctrl_Motor(&CurrentJob);
					break;

				case OPC_REKALIBRIEREN:
					/* Für die Änderung der Kästchenbreite, derzeit noch über defines gelöst */
					break;

				default:
					/* Irgendwas ist schiefgelaufen: id, fehlercode, NACK senden, Job löschen - Fehlersuche einbauen? */
					eState.JOB |= JOB_UNKNOWN_ERROR;
					GOTO_STATE(STATE_ERROR);
					break;
				}
				/* Nachricht an die höherstehende Logik, dass Job Ausgeführt wurde
				 * hierzu übertragen wir die ID des Jobs und enden mit ACK, bzw NACK
				 * letzteres ist allerdings noch nicht Implementiert für die Motorsteuerung
				 */
				if(eState.JOB ^ JOB_UNKNOWN_ERROR){
					insertString(send, (uint8_t*)" JOB_DONE ");
					insertString(send, (uint8_t*)CurrentJob.id);
					insertString(send, (uint8_t*)" ACK ");
					putMsg(&Ringpuffer_TX, send);
					eState.SER |= SER_TX_REQUEST;
					clearString(send);

					GOTO_STATE(STATE_UART_COM);
				}
				else{
					insertString(send, (uint8_t*)" JOB_UNKNOWN_ERROR ");
					insertString(send, (uint8_t*)CurrentJob.id);
					insertString(send, (uint8_t*)" NACK");
					putMsg(&Ringpuffer_TX, send);
					eState.SER |= SER_TX_REQUEST;

					clearString(send);

					GOTO_STATE(STATE_UART_COM);
				}
				jobDone(&CurrentJob);
				break;


				case STATE_NET_COM:
					break;


				case STATE_IDLE:
					// *Queue hat irgendein schrott drin stehn - warum?
					retVal = callJob(&Job_Queue, &CurrentJob);

					switch (retVal){
					case EXIT_SUCCESS:
						GOTO_STATE(STATE_MOT_COM);
						break;

					case JOB_PENDING:
						/* eventuelle Problembehandlung hier einfügen ??? */
						GOTO_STATE(STATE_MOT_COM);
						break;

						/*	case EXIT_FAILURE:
						 Warteschlange ist leer, ERROR-state aufrufen und und
						 * Nachsehen, ob noch Errorflags auf abhandlung warten

						GOTO_STATE(STATE_ERROR);
						break; */

					default:
						ii++;
						if(ii >= 150000){

							insertString(send, (uint8_t*)" IDLE ");
							putMsg(&Ringpuffer_TX, send);
							eState.SER |= SER_TX_REQUEST;
							clearString(send);
							ii = 0;
							GOTO_STATE(STATE_UART_COM);
						}
						retVal = 0;
						break;
					}



					//		 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_1);
					//		 SysCtlDelay(160000);
					//		 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2);
					//		 SysCtlDelay(160000);


					break;
		}
	}
}
