/**
  ******************************************************************************
  * @file           : buttons.h
  * @brief          : Header for buttons.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

//#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"
#include "encoders.h"
#include "shift_registers.h"
#include "axis_to_buttons.h"

typedef uint8_t button_data_t;
typedef uint8_t pov_data_t;

void ButtonProcessState (buttons_state_t * p_button_state, uint8_t * pov_buf, app_config_t * p_config, uint8_t * pos);

void ButtonsCheck (app_config_t * p_config);
void ButtonsGet (uint8_t * raw_data, button_data_t * data);
void POVsGet (pov_data_t * data);


#endif 	/* __BUTTONS_H__ */

