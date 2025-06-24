/**
  ******************************************************************************
  * @file           : simhub.h
  * @brief          : Header for simhub.c file.
  ******************************************************************************
  */
	
	/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CDC_DATA_HANDLER_H__
#define __CDC_DATA_HANDLER_H__

#include "common_types.h"

#define SH_PACKET_SIZE								32
#define MAX_RING_BIF_SIZE 						128//64	// 32?

typedef struct
{
	uint8_t *buffer;
	uint16_t idxIn;
	uint16_t idxOut;
	uint16_t size;
} ring_buf_t;


typedef enum 
{
    RING_ERROR = 0,
    RING_SUCCESS = !RING_ERROR
} RB_ErrorStatus_t;


void RB_Push(uint8_t symbol, ring_buf_t* buf);
uint8_t RB_Pop(ring_buf_t *buf);
uint16_t RB_Size(ring_buf_t *buf);
//int32_t RING_ShowSymbol(uint16_t symbolNumber ,RING_buffer_t *buf);
void RB_Clear(ring_buf_t* buf);
RB_ErrorStatus_t RB_Init(ring_buf_t *ring, uint8_t *buf, uint16_t size);
ring_buf_t * RB_GetPtr(void);


uint8_t SH_ProcessIncomingData(uint8_t *raw_serial_data, uint8_t size);
uint16_t SH_BufferFreeSize(void);

void SH_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length);

#endif 	/* __CDC_DATA_HANDLER_H__ */
