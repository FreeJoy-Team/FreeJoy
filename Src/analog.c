/**
  ******************************************************************************
  * @file           : analog.c
  * @brief          : Analog axis driver implementation
  ******************************************************************************
  */

#include "analog.h"
#include <string.h>
#include "sensors.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim3;
uint16_t input_data[MAX_AXIS_NUM];

analog_data_t scaled_axis_data[MAX_AXIS_NUM];
analog_data_t raw_axis_data[MAX_AXIS_NUM];
analog_data_t out_axis_data[MAX_AXIS_NUM];

uint8_t FILTER_LOW_COEFF[FILTER_LOW_SIZE] = {40, 30, 15, 10, 5};
uint8_t FILTER_MED_COEFF[FILTER_MED_SIZE] = {30, 20, 10, 10, 10, 6, 6, 4, 2, 2};
uint8_t FILTER_HIGH_COEFF[FILTER_HIGH_SIZE] = {20, 20, 10, 10, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1};

uint16_t filter_buffer[MAX_AXIS_NUM][FILTER_HIGH_SIZE];

adc_channel_config_t channel_config[MAX_AXIS_NUM] =
{
	{ADC_CHANNEL_0, 0}, {ADC_CHANNEL_1, 1}, 
	{ADC_CHANNEL_2, 2}, {ADC_CHANNEL_3, 3},
	{ADC_CHANNEL_4, 4}, {ADC_CHANNEL_5, 5}, 
	{ADC_CHANNEL_6, 6}, {ADC_CHANNEL_7, 7}, 
};

// Map function 
static float map2(	float x, 
											float in_min, 
											float in_max, 
											float out_min,
											float out_max)
{
	float tmp8;
	float ret;
	
	tmp8 = x;
	
	
	if (tmp8 < in_min)	return out_min;
	if (tmp8 > in_max)	return out_max;
		
	ret = (tmp8 - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	
	return ret;
}
// Map function with separate action for each half of axis
static float map3(	float x, 
											float in_min, 
											float in_center, 
											float in_max, 
											float out_min,
											float out_center,
											float out_max)
{
	float tmp8;
	float ret;
	
	tmp8 = x;
	
	
	if (tmp8 < in_min)	return out_min;
	if (tmp8 > in_max)	return out_max;
	
	if (tmp8 < in_center)
	{
		ret = ((tmp8 - in_min) * (out_center - out_min) / (in_center - in_min) + out_min);
  }
	else
	{
		ret = ((tmp8 - in_center) * (out_max - out_center) / (in_max - in_center) + out_center);
	}
	return ret;
}

uint16_t SetResolutioin (uint16_t value, uint8_t resolution)
{
	uint16_t tmp = 0;
	uint16_t ret = 0;
	
	if (resolution >= 12)
	{
		return value;
	}
	else if (resolution > 0)
	{
		tmp = value >> (12 - resolution);		// using tmp variable because optimizer 
		ret = tmp << (12 - resolution);
	}
	
	return ret;
}

// FIR function
uint16_t Filter (uint16_t value, uint16_t * filter_buf, filter_t filter_lvl)
{
	uint32_t tmp32;
	
	switch (filter_lvl)
	{
		default:
		case FILTER_NO:
			return value;
		
		case FILTER_LOW:
			tmp32 = value * FILTER_LOW_COEFF[0];
			for (uint8_t i=FILTER_LOW_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LOW_COEFF[i];
			}
		break;
		
		case FILTER_MEDIUM:
			tmp32 = value * FILTER_MED_COEFF[0];
			for (uint8_t i=FILTER_MED_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_MED_COEFF[i];
			}
		break;
		
		case FILTER_HIGH:
			tmp32 = value * FILTER_HIGH_COEFF[0];
			for (uint8_t i=FILTER_HIGH_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_HIGH_COEFF[i];
			}
			
		break;
	}
	
	filter_buf[0] = (uint16_t)(tmp32/100);
	
	
	return filter_buf[0];
}

// Shaping function for axes
uint16_t ShapeFunc (axis_config_t * p_axis_cfg,  uint16_t value, uint16_t fullscale, uint8_t point_cnt)
{
	uint32_t out_min, out_max, step;
	uint16_t in_min, in_max;
	uint8_t min_index;
	uint16_t ret;
	
	step = fullscale/(point_cnt-1);
	min_index = value/step;
	
	if (min_index == 9) min_index = 8; 	// case of input 4095
	
	in_min = min_index*step;
	in_max = (min_index+1)*step;
	
	out_min = p_axis_cfg->curve_shape[min_index] * fullscale/200 + fullscale/2;
	out_max = p_axis_cfg->curve_shape[min_index+1] * fullscale/200 + fullscale/2;
	
	ret = map2(value, in_min, in_max, out_min, out_max);
	
	return(ret);
}

/* Axes init function */
void AxesInit (app_config_t * p_config)
{
	uint8_t adc_cnt = 0;
	uint8_t sensors_cnt = 0;
	uint8_t rank = 1;
  ADC_ChannelConfTypeDef sConfig;

	 /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	hdma_adc1.Instance = DMA1_Channel1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
	
	
	// Count ADC channels
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == AXIS_ANALOG)
		{
			adc_cnt++;
		}
		else if (p_config->pins[i] == TLE5011_CS)
		{
			sensors_cnt++;
		}
	}
	
	if ((adc_cnt + sensors_cnt) > MAX_AXIS_NUM)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
	// Init ADC
	if (adc_cnt > 0)
	{
		hadc1.Instance = ADC1;
		hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
		hadc1.Init.ContinuousConvMode = ENABLE;
		hadc1.Init.DiscontinuousConvMode = DISABLE;
		hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
		hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc1.Init.NbrOfConversion = adc_cnt;
		if (HAL_ADC_Init(&hadc1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
	}
	
	uint8_t axis_num = 0;
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		// Configure Sensors channels		
		if (p_config->pins[i] == TLE5011_CS)
		{
			axis_num++;
		}
	}
	for (int i=0; i<MAX_AXIS_NUM; i++)
	{ 
		if (p_config->pins[i] == AXIS_ANALOG)		// Configure ADC channels
		{
			sConfig.Channel = channel_config[i].channel;
			sConfig.Rank = rank++;
			sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
			}
			axis_num++;
		}
		
	}

	if (adc_cnt > 0)
	{
		if(HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&input_data[sensors_cnt], adc_cnt) != HAL_OK) 
		{
			Error_Handler();
		}
	}
}

void AxesProcess (app_config_t * p_config)
{
	uint16_t tmp16;
	float tmpf;
	uint8_t channel = 0;
	
	// sensors data
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == TLE5011_CS)
		{
			tmpf = 0;
			if (TLE501x_Get(&pin_config[i], &tmpf) == HAL_OK)
			{
				if (p_config->axis_config[channel].magnet_offset)
				{
					tmpf -= 180;
					if (tmpf < -180) tmpf += 360;
					else if (tmpf > 180) tmpf -= 360;
				}
				input_data[channel] = map2(tmpf, -180, 180, 0, 4095);
			}
			channel++;
		}
	}
	// adc data
	for (int i=0; i<MAX_AXIS_NUM; i++)
	{
		if (p_config->pins[i] == AXIS_ANALOG)
		{
			tmp16 = input_data[channel];
			channel++;
		}
	}
	
	
	
			// Process data
	for (int i=0; i<channel; i++)
	{				
			// Filtering
			tmp16 = Filter(input_data[i], filter_buffer[i], p_config->axis_config[i].filter);
			
			// Scale output data
			tmp16 = map3(	tmp16, 
										p_config->axis_config[i].calib_min,
										p_config->axis_config[i].calib_center,		
										p_config->axis_config[i].calib_max, 
										0,
										2047,
										4095);		
			// Shaping
			tmp16 = ShapeFunc(&p_config->axis_config[i], tmp16, 4095, 10);
			// Lowing resolution if needed
			tmp16 = SetResolutioin(tmp16, p_config->axis_config[i].resolution);
		
			// Invertion
			if (p_config->axis_config[i].inverted > 0)
			{
				tmp16 = 4095 - tmp16;
			}
			
			// setting technical axis data
			scaled_axis_data[i] = tmp16;
			raw_axis_data[i] = input_data[i];
			// setting output axis data
			if (p_config->axis_config[i].out_enabled)	out_axis_data[i] = tmp16;
			else	out_axis_data[i] = 0;
		
	}	
}

void AxisResetCalibration (app_config_t * p_config, uint8_t axis_num)
{
	p_config->axis_config[axis_num].calib_max = 0;
	p_config->axis_config[axis_num].calib_min = 4095;
}

void AnalogGet (analog_data_t * out_data, analog_data_t * scaled_data, analog_data_t * raw_data)
{
	if (scaled_data != NULL)
	{
		memcpy(scaled_data, scaled_axis_data, sizeof(scaled_axis_data));
	}
	if (raw_data != NULL)
	{
		memcpy(raw_data, raw_axis_data, sizeof(raw_axis_data));
	}
	if (raw_data != NULL)
	{
		memcpy(out_data, out_axis_data, sizeof(raw_axis_data));
	}
}




