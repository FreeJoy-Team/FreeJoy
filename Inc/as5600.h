/**
  ******************************************************************************
  * @file           : as5600.h
  * @brief          : Header for as5600.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AS5600_H__
#define __AS5600_H__

#include "common_types.h"
#include "periphery.h"

#define AS5600_I2C_ADDR				0x36

void AS5600_Init(sensor_t * sensor);
int16_t AS5600_GetData(sensor_t * sensor);

int AS5600_ReadBlocking(sensor_t * sensor);

#endif 	/* __AS5600_H__ */

