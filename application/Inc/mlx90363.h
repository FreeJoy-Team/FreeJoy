/**
  ******************************************************************************
  * @file           : mlx90363.h
  * @brief          : Header for mlx90363.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MLX90363_H__
#define __MLX90363_H__

#include "common_types.h"
#include "periphery.h"

#define MLX90363_SPI_MODE	1

void MLX90363_Start(sensor_t * sensor);

int MLX90363_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel);

void MLX90363_StartDMA(sensor_t * sensor);
void MLX90363_StopDMA(sensor_t * sensor);

#endif 	/* __MLX90363_H__ */

