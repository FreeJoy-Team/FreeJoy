/**
  ******************************************************************************
  * @file           : mlx90393.h
  * @brief          : Header for mlx90393.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MLX90393_H__
#define __MLX90393_H__

#include "common_types.h"
#include "periphery.h"

#ifndef M_PI
	#define M_PI							3.1415926535897932384626433832795
#endif

#define MLX90393_TIMEOUT		100


void MLX90393_StartBurst(sensor_t * sensor);
void MLX90393_StartDMA(sensor_t * sensor);
void MLX90393_StopDMA(sensor_t * sensor);

#endif 	/* __MLX90393_H__ */

