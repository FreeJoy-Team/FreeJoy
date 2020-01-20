/**
  ******************************************************************************
  * @file           : sensors.h
  * @brief          : Header for sensors.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"

#define PI								3.1415926535
#define TLE501x_TIMEOUT		100

int TLE501x_Get(pin_config_t * p_cs_pin_config, float * data);
//void TLE501x_Read(uint8_t * data, uint8_t addr, uint8_t length);
//void TLE501x_Write(uint8_t * data, uint8_t addr, uint8_t length);

#endif 	/* __SENSORS_H__ */

