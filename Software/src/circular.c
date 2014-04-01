/*
 * circular.c
 *
 *  Created on: 21.10.2013
 *      Author: einball
 */

#include "inc_circular.h"



/*
 * getChar gets the char that's in the Buffer
 * and advances the read pointer. The function
 * returns EXIT_FAILURE if there are no more chars
 * to get or the character in a 32 bit signed int.
 */
uint8_t getChar(Circ_Buf *Buf){

	if((Buf->rdptr + 1) == Buf->wrptr){
		return EXIT_BUFFER_EMPTY;
	}
	if((Buf->rdptr + 1) >= Buf->size){
		if(Buf->wrptr == 0){
			return EXIT_BUFFER_EMPTY;
		}
		else{
			Buf->rdptr = 0;
			return (uint8_t)Buf->buf[Buf->rdptr];
		}
	}
	else {
		Buf->rdptr = Buf->rdptr + 1;
		return (uint8_t)Buf->buf[Buf->rdptr];
	}
}

/*
 *  putChar puts a character in the circular buffer
 *  and advances the write pointer. If the buffer
 *  is full, the function returns an EXIT_FAILURE. If
 *  successful the function returns an EXIT_SUCCESS.
 */

int32_t putChar(Circ_Buf *Buf, uint8_t c){

	if((Buf->wrptr + 1) >= Buf->size){
		if(Buf->rdptr == 0){
			return EXIT_BUFFER_FULL;
		}
		Buf->buf[Buf->wrptr] = c;
		Buf->wrptr = 0;
		return EXIT_SUCCESS;
	}
	if(Buf->rdptr == (Buf->wrptr + 1)){
		return EXIT_FAILURE;
	}
	Buf->buf[Buf->wrptr] = c;
	Buf->wrptr = Buf->wrptr + 1;
	return EXIT_SUCCESS;
}

/*
 * putMsg puts a message into the circular buffer.
 * it returns EXIT_SUCCESS if the message could be
 * inserted into the buffer and EXIT_FAILURE if not.
 */

int32_t putMsg(Circ_Buf *Buf, uint8_t *msg){
	int32_t MsgLength = 0, i = 0, BufSpace = 0;

	BufSpace = getRemBufSpace(Buf);

	//Message length
	while(*(msg+MsgLength) != '\0'){
		MsgLength++;
	}

	if(BufSpace < MsgLength){
		return EXIT_FAILURE;
	}
	else{
		while(MsgLength > 0){
			if(!putChar(Buf, *(msg + i))){
				return EXIT_FAILURE;
			}
			MsgLength--;
			i++;
		}
		return EXIT_SUCCESS;
	}
}


/*
 * The function returns the remaining space
 * that's left in the buffer Buf. It returns
 * EXIT_FAILURE if the rdptr is the same as
 * the wrptr and therefore the buffer is
 * inconsistent!
 */
int32_t getRemBufSpace(Circ_Buf *Buf){
	int32_t i = 0;

	if(Buf->rdptr > Buf->wrptr){
		i = Buf->size - Buf->wrptr + (Buf->size - Buf->rdptr) - 1;
	}
	if(Buf->rdptr < Buf->wrptr){
		i = Buf->size - Buf->wrptr + Buf->rdptr - 2;
	}
	if((Buf->rdptr + 1) >= Buf->size){
		if(Buf->wrptr == 0) return Buf->size - 1;
	}
	return i;
}

/*
 * getMsg returns a pointer to the array
 * in which the message has been placed
 * or NULL if the buffer is too short for
 * the given message.
 */

uint8_t getMsg(Circ_Buf *Buf, uint8_t *MsgBuf, uint16_t BufSize){
	int32_t c, i = 0;
	c = getChar(Buf);
	while(c != '\n'){
		if(c==EXIT_BUFFER_EMPTY){
			return EXIT_BUFFER_EMPTY;
		}
		MsgBuf[i] = c;
		i++;
		if(i >= (BufSize - 2)) return EXIT_BUFFER_SIZE;
		c = getChar(Buf);
	}

	/*
	 * Since we left the while before adding the \n and the
	 * string end we need to add it now!
	 */
	MsgBuf[i] = '\n';
	MsgBuf[i+1] = '\0';
	return EXIT_SUCCESS;
}

