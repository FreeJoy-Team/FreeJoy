/**
  ******************************************************************************
  * @file           : sensors.h
  * @brief          : Header for sensors.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "common_types.h"
#include "periphery.h"

#define M_PI							3.1415926535897932384626433832795
#define TLE501x_TIMEOUT		100

int TLE501x_Get(pin_config_t * p_cs_pin_config, float * data);

#endif 	/* __SENSORS_H__ */

