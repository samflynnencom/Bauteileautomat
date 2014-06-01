/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
#include "Board.h"



void FXN_uart_callback(UART_Handle handle, void *buf, int count);

UART_Params uartParams;	// Parameter struct for UART0
UART_Handle uart;		// UART driver handle
uint8_t uartRxBuf[256];
#define UART_NEWLINE 	 "\n"
#define UART_CMDLINE 	 "~: "


/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void TSK_heartBeatFxn(UArg arg0, UArg arg1)
{
	UART_write(uart, UART_CMDLINE, sizeof(UART_CMDLINE) - 1);	// Print commandline
	UART_read(uart, uartRxBuf, sizeof(uartRxBuf));			// Read from the UART once to start a continuous read

    while (1) {
        Task_sleep((UInt)arg0);
        GPIO_toggle(Board_LED0);
	}
}

void TSK_handleUartRXFxn(UArg arg0, UArg arg1){
	uint16_t i 							= 0;
	uint8_t UART_NEWLINE_FOUND		 	= 0;
	uint8_t UART_NO_NEWLINE_PRESENT 	= 0;

	while(1){
		Semaphore_pend(sem_uart_rxdone, BIOS_WAIT_FOREVER);	// Wait for message to arrive

		i 							= 0;
		UART_NO_NEWLINE_PRESENT 	= 0;
		UART_NEWLINE_FOUND 			= 0;

		while(!UART_NEWLINE_FOUND && !UART_NO_NEWLINE_PRESENT){		// Prevent i from overflowing the buffer
			if(i >= sizeof(uartRxBuf)){
				UART_NO_NEWLINE_PRESENT = 1;
				#ifdef DEBUG
				System_printf("Error in UART Message handling: No newline character found!\n");
				System_flush();
				#endif
			}
			if(uartRxBuf[i] == '\n'){
				UART_NEWLINE_FOUND = 1;
			}
			else{
				i = i + 1;
			}
		}

		if(!UART_NO_NEWLINE_PRESENT && (uartRxBuf[0] != '\n')){
			// Parse
			//UART_write(uart, uartRxBuf, i);
			if(strcmp((char*)uartRxBuf , "SET RED LED ON\n")){
				 GPIO_write(Board_LED1,Board_LED_ON);
				 UART_write(uart, "RED LED IS ON",sizeof("RED LED IS ON"));
			}
			if(uartRxBuf == "SET RED LED OFF\n"){
				 GPIO_write(Board_LED1,Board_LED_OFF);
				 UART_write(uart, "RED LED IS OFF",sizeof("RED LED IS OFF"));
			}
			UART_write(uart, UART_NEWLINE, sizeof(UART_NEWLINE) - 1);
			UART_write(uart, UART_CMDLINE, sizeof(UART_CMDLINE) - 1);
			for(i = 0; i < sizeof(uartRxBuf);i++) uartRxBuf[i] = 0;
		}
		else{
			if(UART_NO_NEWLINE_PRESENT){
				System_printf("Error in UART Message handling: No Newline present!\n Buffer contents ' %s '\n",uartRxBuf);
				System_flush();
				UART_write(uart, UART_CMDLINE, sizeof(UART_CMDLINE) - 1);
			}
			for(i = 0; i < sizeof(uartRxBuf);i++) uartRxBuf[i] = 0;
		}
		UART_read(uart, uartRxBuf, sizeof(uartRxBuf));
	}
}



/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    // Board_initDMA();
    // Board_initI2C();
    // Board_initSPI();
    Board_initUART();
    // Board_initUSB(Board_USBDEVICE);


    /* Initialize drivers */
    UART_Params_init(&uartParams);
    uartParams.readMode 		= UART_MODE_CALLBACK;
    uartParams.writeMode 		= UART_MODE_BLOCKING;
    uartParams.readCallback 	= &FXN_uart_callback;
    uartParams.writeDataMode 	= UART_DATA_BINARY;
    uartParams.readDataMode 	= UART_DATA_TEXT;
    uartParams.readReturnMode 	= UART_RETURN_NEWLINE;
    uartParams.readEcho	 		= UART_ECHO_OFF;
    uartParams.baudRate 		= 9600;
    uartParams.dataLength		= UART_LEN_8;
    uartParams.stopBits			= UART_STOP_ONE;
    uartParams.parityType		= UART_PAR_NONE;
    uart = UART_open(Board_UART0, &uartParams);

    /* SPI */

    /* Ethernet */

    /* End init drivers */

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    System_printf("Inits done. Starting BIOS\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

void FXN_uart_callback(UART_Handle handle, void *buf, int count){
	Semaphore_post(sem_uart_rxdone);

}
