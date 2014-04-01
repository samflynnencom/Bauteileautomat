/*
 * inc_msg.h
 *
 *  Created on: 19.10.2013
 *      Author: einball
 */

#ifndef INC_MSG_H_
#define INC_MSG_H_


#include "main.h"

//Work in progress!

typedef struct Job {
	uint32_t id;
	uint32_t opCode;
	uint32_t coordX;
	uint32_t coordY;
	uint32_t coordZ;
	uint32_t JOINT;
	uint32_t GRIPPER;
	uint32_t opt1;
	uint32_t opt2;
	uint32_t opt3;
	uint32_t crc;
} sJob;

typedef struct JobQueue {

	sJob Job[10];
} sJobQueue;

//Message Teil
uint32_t checkMsgIntegrity();
uint32_t decodeMsg(sJobQueue *Job_Queue, sJob *Buf, uint8_t *MsgPointer);
uint32_t validateMsg(sJob *Job);


//Job Teil
uint32_t addJob(sJobQueue *Queue, sJob *Job);
uint32_t callJob(sJobQueue *Queue, sJob *Job);
uint32_t jobDone(sJob *Job);

#endif /* INC_MSG_H_ */
