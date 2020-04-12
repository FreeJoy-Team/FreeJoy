/**
  ******************************************************************************
  * @file           : ads1115.h
  * @brief          : Header for ads1115.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADS1115_H__
#define __ADS1115_H__

#include "common_types.h"
#include "periphery.h"

void ADS1115_Init(sensor_t * sensor);
int16_t ADS1115_GetData(sensor_t * sensor, uint8_t channel);
int ADS1115_StartDMA(sensor_t * sensor, uint8_t channel);
int ADS1115_SetMuxDMA(sensor_t * sensor, uint8_t channel);

#endif 	/* __ADS1115_H__ */

