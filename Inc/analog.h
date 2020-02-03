/**
  ******************************************************************************
  * @file           : analog.h
  * @brief          : Header for analog.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ANALOG_H__
#define __ANALOG_H__

//#include "stm32f1xx_hal.h"
#include "common_types.h"
#include "periphery.h"

#define FILTER_LOW_SIZE				5
#define FILTER_MED_SIZE				10
#define FILTER_HIGH_SIZE			20

extern tle_t sensors[MAX_AXIS_NUM];

typedef struct
{
	uint32_t 	channel;
	uint8_t 	number;
	
} adc_channel_config_t;



void AxesInit (app_config_t * p_config);
void AxesProcess (app_config_t * p_config);
void AxisResetCalibration (app_config_t * p_config, uint8_t axis_num);
void AnalogGet (analog_data_t * out_data, analog_data_t * scaled_data, analog_data_t * raw_data);

#endif 	/* __ANALOG_H__ */

