/**
  ******************************************************************************
  * @file           : axis_to_buttons.h
  * @brief          : Header for axis_to_buttons.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AXIS_TO_BUTTONS_H__
#define __AXIS_TO_BUTTONS_H__

#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"
#include "analog.h"

#define AXIS_FULLSCALE			4095
#define SWICTH_DEAD_ZONE		15

void AxesToButtonsProcess (buttons_state_t * button_state_buf, app_config_t * p_config, uint8_t * pos);

#endif 	/* __AXIS_TO_BUTTONS_H__ */

