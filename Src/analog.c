/**
  ******************************************************************************
  * @file           : analog.c
  * @brief          : Analog axis driver implementation
  ******************************************************************************
  */

#include "analog.h"
#include <string.h>
#include "flash.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim3;
uint16_t adc_data[MAX_AXIS_NUM];
analog_data_t axis_data[MAX_AXIS_NUM];

uint8_t FILTER_LOW_COEFF[FILTER_LOW_SIZE] = {45, 30, 15, 5, 5};
uint8_t FILTER_MED_COEFF[FILTER_MED_SIZE] = {30, 20, 10, 10, 10, 6, 6, 4, 2, 2};
uint8_t FILTER_HIGH_COEFF[FILTER_HIGH_SIZE] = {25, 15, 10, 10, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1};

uint16_t filter_buffer[MAX_AXIS_NUM][FILTER_HIGH_SIZE];

adc_channel_config_t channel_config[MAX_AXIS_NUM] =
{
	{ADC_CHANNEL_0, 0}, {ADC_CHANNEL_1, 1}, 
	{ADC_CHANNEL_2, 2}, {ADC_CHANNEL_3, 3},
	{ADC_CHANNEL_4, 4}, {ADC_CHANNEL_5, 5}, 
	{ADC_CHANNEL_6, 6}, {ADC_CHANNEL_7, 7}, 
};

// Map function with separate action for each half of axis
static uint32_t map(uint32_t x, 
										uint32_t in_min, 
										uint32_t in_center, 
										uint32_t in_max, 
										uint32_t out_min,
										uint32_t out_center,
										uint32_t out_max)
{
	uint32_t tmp8;
	uint32_t ret;
	
	tmp8 = x;
	
	
	if (tmp8 < in_min)	return in_min;
	if (tmp8 > in_max)	return in_max;
	
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

uint16_t filter (uint16_t value, uint16_t * filter_buf, filter_t filter_lvl)
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

/* ADC init function */
void ADC_Init (app_config_t * p_config)
{
	uint8_t channels_cnt = 0;
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
		if (p_config->pins[i] == AXIS_ANALOG || p_config->pins[i] == AXIS_ANALOG)
		{
			channels_cnt++;
		}
	}
	if (channels_cnt > MAX_AXIS_NUM)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
	// Init ADC
	if (channels_cnt > 0)
	{
		hadc1.Instance = ADC1;
		hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
		hadc1.Init.ContinuousConvMode = ENABLE;
		hadc1.Init.DiscontinuousConvMode = DISABLE;
		hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
		hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc1.Init.NbrOfConversion = channels_cnt;
		if (HAL_ADC_Init(&hadc1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
	}
	
	// Configure ADC channels
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == AXIS_ANALOG || p_config->pins[i] == AXIS_ANALOG)
		{
			sConfig.Channel = channel_config[i].channel;
			sConfig.Rank = channel_config[i].number+1;
			sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
			if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
			{
				_Error_Handler(__FILE__, __LINE__);
			}
			
			// reset precalibrated values at startup if autocalibration set
			if (p_config->axis_config[channel_config[i].number].autocalib)
			{
				AxisResetCalibration(p_config, channel_config[i].number);
			}
		}
	}

	if(HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adc_data[0],channels_cnt) != HAL_OK) 
	{
		Error_Handler();
	}
}

void AnalogProcess (app_config_t * p_config)
{
	uint16_t tmp16;
	
	for (int i=0; i<MAX_AXIS_NUM; i++)
	{
		tmp16 = adc_data[i];
		
		if (p_config->axis_config[i].autocalib)
		{
			// Update calib data
			if (tmp16 > p_config->axis_config[i].calib_max)
			{
				p_config->axis_config[i].calib_max = tmp16;
			}
			else if (tmp16 < p_config->axis_config[i].calib_min)
			{
				p_config->axis_config[i].calib_min = tmp16;
			}
			
			tmp16 = (p_config->axis_config[i].calib_max + p_config->axis_config[i].calib_min)/2;
			
			if (p_config->axis_config[i].calib_center != tmp16 )
			{				
				p_config->axis_config[i].calib_center = tmp16;
			}
		}
		
		// filter
		tmp16 = filter(adc_data[i], filter_buffer[i], p_config->axis_config[i].filter);
		
		// Scale output data
		tmp16 = map(	tmp16, 
									p_config->axis_config[i].calib_min,
									p_config->axis_config[i].calib_center,		
									p_config->axis_config[i].calib_max, 
									0,
									2047,
									4095);
		
		// TODO: Shapes
		// analog_data[i] = ShapeFunc(i, tmp16);
		axis_data[i] = tmp16;
	}	
}

void AxisResetCalibration (app_config_t * p_config, uint8_t axis_num)
{
	p_config->axis_config[axis_num].calib_max = 0;
	p_config->axis_config[axis_num].calib_min = 4095;
}

void AnalogGet (analog_data_t * scaled_data, analog_data_t * raw_data)
{
	if (scaled_data != NULL)
	{
		memcpy(scaled_data, axis_data, sizeof(axis_data));
	}
	if (raw_data != NULL)
	{
		memcpy(raw_data, adc_data, sizeof(adc_data));
	}
}




