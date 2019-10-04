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

typedef struct buttons_state_t
{
	uint8_t prev_state;
	uint8_t current_state;
	uint8_t changed;
	uint64_t time_last;	
	uint8_t cnt;
	
} buttons_state_t;



typedef uint8_t button_data_t;
typedef uint8_t pov_data_t;

void ButtonsCheck (app_config_t * p_config);
void ButtonsGet (button_data_t * data);
void POVsGet (pov_data_t * data);


#endif 	/* __BUTTONS_H__ */

