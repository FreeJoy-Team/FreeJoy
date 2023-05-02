/**
  ******************************************************************************
  * @file           : cdc_data_handler.h
  * @brief          : Header for cdc_data_handler.c file.
  ******************************************************************************
  */
	
	/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CDC_DATA_HANDLER_H__
#define __CDC_DATA_HANDLER_H__

#include "common_types.h"


#define SH_MESSAGE_HEADER							0x03
#define SH_PACKET_SIZE								32
#define SH_VERSION 										'j'

#define MAX_RING_BIF_SIZE 						64	// max 255 because uint8_t
typedef struct
{
	uint8_t size;
	uint8_t read_index;
	uint8_t buffer[MAX_RING_BIF_SIZE];
} ring_buf_t;
/* DONT USE DIRECTLY */
uint8_t RB_WriteIndex(const ring_buf_t *b);
/* Return the size of the buffer */
uint8_t RB_Size(ring_buf_t *b);
/* Access the buffer using array syntax, not interrupt safe */
uint8_t RB_MaxSize(ring_buf_t *b);
/* Return true if the buffer is full */
uint8_t RB_IsFull(ring_buf_t *b);
/* Return true if the buffer is empty */
uint8_t RB_IsEmpty(ring_buf_t *b);
/* Reset the buffer to an empty state */
void RB_Clear(ring_buf_t *b);
/* Push a data at the end of the buffer. Return 1 if success, otherwise 0*/
uint8_t RB_Push(uint8_t value, ring_buf_t *b);
/* Push a array data at the end of the buffer. Return 1 if success, otherwise 0*/
uint8_t RB_PushArr(uint8_t *arr, uint8_t length, ring_buf_t *b);
/* Pop the data at the beginning of the buffer and return it */
uint8_t RB_Pop(ring_buf_t *b);
/* Pop the data at the beginning of the buffer. Return 1 if success, otherwise 0 */
uint8_t RB_PopErrCheck(uint8_t *value, ring_buf_t *b);
/* Return pointer to Ring Buffer */
ring_buf_t * RB_GetPtr(void);

void SH_ProcessIncomingData(uint8_t *data, uint8_t size, ring_buf_t *buffer);

void SH_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length);

#endif 	/* __CDC_DATA_HANDLER_H__ */
