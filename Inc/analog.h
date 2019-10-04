/**
  ******************************************************************************
  * @file           : analog.h
  * @brief          : Header for analog.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"

typedef struct
{
	uint32_t 	channel;
	uint8_t 	number;
	
} adc_channel_config_t;

typedef uint16_t analog_data_t;

void ADC_Init (app_config_t * p_config);
void AnalogProcess (app_config_t * p_config);
void AxisResetCalibration (app_config_t * p_config, uint8_t axis_num);
void AnalogGet (analog_data_t * data);

#endif 	/* __ANALOG_H__ */

