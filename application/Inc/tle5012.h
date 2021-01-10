/**
  ******************************************************************************
  * @file           : tle5012.h
  * @brief          : Header for tle5012.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TLE5012_H__
#define __TLE5012_H__

#include "common_types.h"
#include "periphery.h"

#ifndef M_PI
	#define M_PI							3.1415926535897932384626433832795
#endif

#define TLE5012_SPI_MODE			1

void TLE5012_StartDMA(sensor_t * sensor);
void TLE5012_StopDMA(sensor_t * sensor);
int TLE5012_GetAngle(sensor_t * sensor, float * angle);

#endif 	/* __TLE5012_H__ */

