/*
 * startup.c
 *
 *  Created on: 21.10.2013
 *      Author: einball
 */
#include "inc_startup.h"


void setup_INT(){
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	UARTIntEnable(UART0_BASE, UART_INT_OE | UART_INT_BE);
	UARTIntEnable(UART0_BASE, UART_INT_PE | UART_INT_PE);
	UARTIntEnable(UART0_BASE, UART_INT_TX);

	IntMasterEnable();

}

void setup_GPIO(){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);	//LEDs als Output deklarieren
	//der genaue input Port/Pin muss noch angepasst werden
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Auf Port A liegen die UART Pins
}


void setup_UART(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);		//Enable UART Ports
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	//Rx und Tx deklarieren
    UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);		// Tx Interrupt wird mit End-of-Transmission ausgelöst: Teil der schnellen Dörte Lösung, siehe: Init_Transmission
}

void setup_SPI(){

}

void setup_JOB_QUEUE(sJobQueue *Queue){
	uint32_t i;
/* Die Job-warteschlange wird hier mit 0 initialisiert, allerdings geht so natürlich auch beim Systemneustart alles verloren */
	for(i = 0; i<10; i++){
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
}


void setup_HOME_POS(){
	/*
	 * GoUntil(MOT_JOINT,0,0,MIN_SPEED)
	 * GoUntil(MOT_GRIPPER,0,1,MIN_SPEED)
	 * GoUntil(MOT_Z_AXIS,0,0,MIN_SPEED),
	 * GoUntil(MOT_Y_AXIS,0,0,MIN_SPEED)
	 * GoUntil(MOT_X_AXIS,0,0,MIN_SPEED)
	 */
}
