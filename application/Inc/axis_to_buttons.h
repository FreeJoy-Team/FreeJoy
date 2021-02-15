/**
  ******************************************************************************
  * @file           : axes_to_buttons.h
  * @brief          : Header for axes_to_buttons.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AXES_TO_BUTTON_H__
#define __AXES_TO_BUTTON_H__

#include "common_types.h"
#include "periphery.h"
#include "analog.h"


void AxisToButtonsGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos);

#endif 	/* __AXES_TO_BUTTON_H__ */

