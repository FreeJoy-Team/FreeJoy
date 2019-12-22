/**
  ******************************************************************************
  * @file           : buttons.h
  * @brief          : Header for buttons.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"
#include "encoders.h"
#include "axis_to_buttons.h"

typedef uint8_t button_data_t;
typedef uint8_t pov_data_t;

void ButtonsCheck (app_config_t * p_config);
void ButtonsGet (button_data_t * data);
void POVsGet (pov_data_t * data);


#endif 	/* __BUTTONS_H__ */

