/**
  ******************************************************************************
  * @file           : leds.h
  * @brief          : Header for leds.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDS_H__
#define __LEDS_H__

#include "common_types.h"
#include "periphery.h"

extern external_led_data_t external_led_data;

void LEDs_PhysicalProcess (dev_config_t * p_dev_config);

#endif 	/* __LEDS_H__ */

