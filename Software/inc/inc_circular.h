/*
 * inc_circular.h
 *
 *  Created on: 21.10.2013
 *      Author: einball
 */

#ifndef INC_CIRCULAR_H_
#define INC_CIRCULAR_H_

#include <stdint.h>
#include <stdlib.h>
#include "inc_def.h"

#define 		EXIT_BUFFER_FULL	0x02
#define			EXIT_MSG_PENDING	0x03
#define			EXIT_BUFFER_EMPTY	0x04
#define			EXIT_MSG_NOT_VALID	0x05
#define			EXIT_MSG_IS_VALID	0x06
#define			EXIT_BUFFER_SIZE	0x07

/* Already defined them
 * #define		EXIT_SUCCESS
 * #define		EXIT_FAILURE
 */



/* Do not forget to initialize the Buffers with rdptr == wrptr! */
typedef struct Buf {
	uint8_t buf[BUF_SIZE];
	uint32_t rdptr;
	uint32_t wrptr;
	uint32_t size;
} Circ_Buf;

uint8_t getMsg(Circ_Buf *Buf, uint8_t *MsgBuf, uint16_t BufSize);
int32_t getRemBufSpace(Circ_Buf *Buf);
int32_t putMsg(Circ_Buf *Buf, uint8_t *msg);
int32_t putChar(Circ_Buf *Buf, uint8_t c);
uint8_t getChar(Circ_Buf *Buf);


#endif /* INC_CIRCULAR_H_ */
