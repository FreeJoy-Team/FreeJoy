/**
  ******************************************************************************
  * @file           : tle5011.h
  * @brief          : Header for tle5011.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TLE5011_H__
#define __TLE5011_H__

#include "common_types.h"
#include "periphery.h"

#ifndef M_PI
	#define M_PI							3.1415926535897932384626433832795
#endif

#define TLE5011_SPI_MODE			1

void TLE501x_StartDMA(sensor_t * sensor);
void TLE501x_StopDMA(sensor_t * sensor);
int TLE501x_GetAngle(sensor_t * sensor, float * angle);

#endif 	/* __TLE5011_H__ */

