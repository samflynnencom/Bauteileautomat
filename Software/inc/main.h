/*
 * main.h
 *
 *  Created on: 28.10.2013
 *      Author: einball
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>



/* System states */
typedef enum {
					STATE_STARTUP,
					STATE_ERROR,
					STATE_MSG_HANDLING,
					STATE_UART_COM,
					STATE_MOT_COM,
					STATE_NET_COM,
					STATE_LCD_COM,
					STATE_JOB_HANDLING,
					STATE_IDLE

} sysState_t;

sysState_t SYS_STATE;


/* Error States */
typedef struct sError {

	uint32_t	SER;
	uint32_t	STA;
	uint32_t	ETH;
	uint32_t	MSG;
	uint32_t	MOT;
	uint32_t	JOB;
} sErrorFlags;

sErrorFlags eState;

/* Prototypes */
void GOTO_STATE(sysState_t state);

#endif /* MAIN_H_ */
