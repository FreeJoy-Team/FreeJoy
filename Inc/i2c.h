/**
  ******************************************************************************
  * @file           : i2c.h
  * @brief          : Header file for i2c.h                 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void HardI2C_Init(void);
void HardI2C_Transmit(uint8_t * data, uint16_t length);
void HardI2C_Receive(uint8_t * data, uint16_t length);
#endif 	/* __I2C_H__ */

