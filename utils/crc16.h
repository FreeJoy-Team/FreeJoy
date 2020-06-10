/**
  ******************************************************************************
  * @file           : crc16.h
  * @brief          : Header for crc16.c file.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC16_H__
#define __CRC16_H__
	
#include "stm32f10x.h"

#define POLINOM   0xA001

uint16_t Crc16( uint8_t *buf, uint16_t num );

#endif /* __CRC16_H__ */
