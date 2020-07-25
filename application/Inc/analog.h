/**
  ******************************************************************************
  * @file           : analog.h
  * @brief          : Header for analog.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "common_types.h"
#include "periphery.h"

#define FILTER_BUF_SIZE									20
#define DEADBAND_BUF_SIZE								8
#define DEADBAND_HOLD_VALUE							2000

#define ADC_CONV_NUM										8

extern sensor_t sensors[MAX_AXIS_NUM];

typedef struct
{
	uint32_t 	channel;
	uint8_t 	number;
	
} adc_channel_config_t;



void AxesInit (dev_config_t * p_dev_config);
void ADC_Conversion (void);
void AxesProcess (dev_config_t * p_dev_config);
void AxisResetCalibration (dev_config_t * p_dev_config, uint8_t axis_num);
void AnalogGet (analog_data_t * out_data, analog_data_t * scaled_data, analog_data_t * raw_data);

#endif 	/* __ANALOG_H__ */

