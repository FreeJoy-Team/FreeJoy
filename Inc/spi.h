/**
  ******************************************************************************
  * @file           : spi.h
  * @brief          : Header file for spi.h                 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"


#define SPI_USE_DMA			1

void HardSPI_Init(void);
void HardSPI_HalfDuplex_Transmit(uint8_t * data, uint16_t length);
void HardSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length);

#endif 	/* __SPI_H__ */

