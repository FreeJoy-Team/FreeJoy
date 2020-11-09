/**
  ******************************************************************************
  * @file           : as5048a.h
  * @brief          : Header for as5048a.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AS5048A_H__
#define __AS5048A_H__

#include "common_types.h"
#include "periphery.h"

#define AS5048A_SPI_MODE							1

void AS5048A_StartDMA(sensor_t * sensor);
int AS5048A_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel);
void AS5048A_StopDMA(sensor_t * sensor);

#endif 	/* __AS5048A_H__ */

