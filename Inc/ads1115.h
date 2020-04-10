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

void ADS1115_Start(sensor_t * sensor);
void ADS1115_StartDMA(sensor_t * sensor);
int ADS1115_GetData(uint16_t * data, sensor_t * sensor);
void ADS1115_StopDMA(sensor_t * sensor);

#endif 	/* __ADS1115_H__ */

