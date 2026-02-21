/**
  ******************************************************************************
  * @file           : led_effects.h
  * @brief          : Header for led_effects.c file.
  ******************************************************************************
  */
	
#ifndef _LED_EFFECTS_H
#define _LED_EFFECTS_H

#include "common_types.h"

//void LedEffect_Init(dev_config_t * p_dev_config);

void ArgbLed_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length, int32_t current_ticks);
//void SetEffect(RGB_t *rgb, unsigned count, uint8_t effect);
//void UpdateLEDs(void);

#endif
