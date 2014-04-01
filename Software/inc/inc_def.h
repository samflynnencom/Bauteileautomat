/*
 * defines.h
 *
 *  Created on: 18.10.2013
 *      Author: einball
 */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Success of fail */
#undef		EXIT_SUCCESS
#undef		EXIT_FAILURE
#define 	EXIT_SUCCESS						1
#define 	EXIT_FAILURE						0


/* Motorzuordnung zur SPI Chain */
#define		MOT_X_AXIS_1					0x00000000
#define		MOT_X_AXIS_2					0x00000001
#define		MOT_Y_AXIS						0x00000002
#define		MOT_Z_AXIS						0x00000003
#define		MOT_JOINT						0x00000004
#define		MOT_GRIPPER						0x00000005

/* Definition der opCodes */

#define OPC_AUSGABE 						0x00000000
#define OPC_NACHFUELLEN				 		0x00000001
#define OPC_FERNSTEUERN						0x00000002
#define OPC_REKALIBRIEREN					0x00000004

/* Maximale Koordinaten, begrenzt durch Automatengröße, X und Z weiß ich grad nicht auswendig, ist noch zu klären */

#define Max_Pos_X		300
#define Max_Pos_Y		300
#define Max_Pos_Z		3
#define Max_Pos_GRIPPER	3 	//maximale GRIPPER stellung -> hängt direkt mit boxsize zusammen

//X und Y sind noch festzulegen, Z ist immer der letztmögliche Platz

#define FREE_SPACE_JOINT 0

#define FREE_SPACE_X_1	1
#define FREE_SPACE_Y_1 	1
#define FREE_SPACE_Z_1 	3

#define FREE_SPACE_X_2 	1
#define FREE_SPACE_Y_2 	2
#define FREE_SPACE_Z_2 	3

/* Länge der Puffer vom Typ Circ_Buf */
#define BUF_SIZE		1024

/* Länge der Puffer, in denen die Message bearbeitet wird */
#define MSG_BUF_SIZE	64

/* Job Error states that can be passed to and returned from various functions */

#define		JOB_ALL_OK						0x00000000
#define		JOB_RECEIVE_ERROR				0x00000001
#define		JOB_ADD_ERROR					0x00000002
#define		JOB_ADD_SUCCESS					0x00000004
#define		JOB_ADD_PENDING					0x00000008
#define		JOB_PENDING						0x00000010
#define		JOB_UNKNOWN_ERROR				0x00000020
#define		JOB_UNDEFINED8					0x00000040
#define		JOB_UNDEFINED9					0x00000080

/* UART Error states that can be passed to and returned from various functions */
#define		SER_ALL_OK						0x00000000
#define		SER_RX_MESSAGE_COMPLETE			0x00000001
#define		SER_RX_TIMEOUT					0x00000002
#define		SER_RX_IDLE						0x00000004
#define		SER_TX_TRANSFER_COMPLETE		0x00000008
#define		SER_TX_IDLE						0x00000010
#define		SER_RX_ERROR					0x00000020
#define		SER_TX_ERROR					0x00000040
#define		SER_RX_BUFFER_OVERRUN			0x00000080
#define		SER_RX_DECODE_VALID_MSG			0x00000100
#define		SER_UNDEFINED_ERROR				0x00000200
#define		SER_TX_REQUEST					0x00000400
#define		SER_RX_BUFFER_EMPTY				0x00000800

/* (Soft-)SPI Error states that can be passed to and from various functions */
#define		SPI_ALL_OK						0x00000000
#define		SPI_UNDEFINED2					0x00000001
#define		SPI_UNDEFINED3					0x00000002
#define		SPI_UNDEFINED4					0x00000004
#define		SPI_UNDEFINED5					0x00000008
#define		SPI_UNDEFINED6					0x00000010
#define		SPI_UNDEFINED7					0x00000020
#define		SPI_UNDEFINED8					0x00000040
#define		SPI_UNDEFINED9					0x00000080


/* Statemachine Error states that can be passed to and returned from various functions */
#define		STA_ALL_OK						0x00000000
#define		STA_UNDEFINED2					0x00000001
#define		STA_UNDEFINED3					0x00000002
#define		STA_UNDEFINED4					0x00000004
#define		STA_UNDEFINED5					0x00000008
#define		STA_UNDEFINED6					0x00000010
#define		STA_UNDEFINED7					0x00000020
#define		STA_UNDEFINED8					0x00000040
#define		STA_UNDEFINED9					0x00000080



/* Ethernet Error states that can be passed to and returned from various functions */
#define		ETH_ALL_OK						0x00000000
#define		ETH_UNDEFINED2					0x00000001
#define		ETH_UNDEFINED3					0x00000002
#define		ETH_UNDEFINED4					0x00000004
#define		ETH_UNDEFINED5					0x00000008
#define		ETH_UNDEFINED6					0x00000010
#define		ETH_UNDEFINED7					0x00000020
#define		ETH_UNDEFINED8					0x00000040
#define		ETH_UNDEFINED9					0x00000080



/* Message system Error states that can be passed to and returned from various functions */
#define		MSG_ALL_OK						0x00000000
#define		MSG_LENGTH_ERROR				0x00000001
#define		MSG_INVALID						0x00000002
#define		MSG_CRC_ERROR					0x00000004
#define		MSG_ID_ERROR					0x00000008
#define		MSG_UNKNOWN_ERROR				0x00000010
#define		MSG_CODE_ERROR					0x00000020
#define		MSG_UNDEFINEDo					0x00000040
#define		MSG_UNDEFINEDp					0x00000080



/* Digital Input macros */
#define 	DIN_0_VAL 						(*((volatile uint32_t *)(GPIO_PORTB_BASE + (GPIO_O_DATA + ( 0x20 << 2 )))) >> 5)
#define 	DIN_1_VAL 						(*((volatile uint32_t *)(GPIO_PORTB_BASE + (GPIO_O_DATA + ( 0x80 << 2 )))) >> 7)
#define 	DIN_2_VAL 						(*((volatile uint32_t *)(GPIO_PORTF_BASE + (GPIO_O_DATA + ( 0x10 << 2 )))) >> 4)
#define 	DIN_3_VAL 						(*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + ( 0x08 << 2 )))) >> 3)
#define 	DIN_4_VAL 						(*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + ( 0x04 << 2 )))) >> 2)
#define 	DIN_5_VAL 						(*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + ( 0x02 << 2 )))) >> 1)
#define 	DIN_6_VAL 						(*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + ( 0x01 << 2 )))) >> 0)
#define 	DIN_7_VAL 						(*((volatile uint32_t *)(GPIO_PORTD_BASE + (GPIO_O_DATA + ( 0x80 << 2 )))) >> 7)
#define 	DIN_8_VAL 						(*((volatile uint32_t *)(GPIO_PORTC_BASE + (GPIO_O_DATA + ( 0x80 << 2 )))) >> 7)
#define 	DIN_9_VAL 						(*((volatile uint32_t *)(GPIO_PORTC_BASE + (GPIO_O_DATA + ( 0x40 << 2 )))) >> 6)



/* Motor fault macros */

#define		MOT_0_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTA_BASE + (GPIO_O_DATA + ( 0x80 << 2 )))) >> 7)) & 0x01)
#define		MOT_1_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTA_BASE + (GPIO_O_DATA + ( 0x40 << 2 )))) >> 6)) & 0x01)
#define		MOT_2_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTC_BASE + (GPIO_O_DATA + ( 0x10 << 2 )))) >> 4)) & 0x01)
#define		MOT_3_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTC_BASE + (GPIO_O_DATA + ( 0x20 << 2 )))) >> 5)) & 0x01)
#define		MOT_4_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + ( 0x20 << 2 )))) >> 5)) & 0x01)
#define		MOT_5_FLAG						((~(*((volatile uint32_t *)(GPIO_PORTA_BASE + (GPIO_O_DATA + ( 0x10 << 2 )))) >> 4)) & 0x01)




#endif /* DEFINES_H_ */
