/**
  ******************************************************************************
  * @file           : analog.c
  * @brief          : Analog axis driver implementation
		
		FreeJoy software for game device controllers
    Copyright (C) 2020  Yury Vostrenkov (yuvostrenkov@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
		
  ******************************************************************************
  */

#include "analog.h"

#include "SEGGER_SYSVIEW.h"

#include <string.h>
#include <math.h>
#include "tle5011.h"
#include "mcp320x.h"
#include "mlx90393.h"
#include "ads1115.h"
#include "as5600.h"
#include "buttons.h"
#include "encoders.h"

sensor_t sensors[MAX_AXIS_NUM];
analog_data_t input_data[MAX_AXIS_NUM];

analog_data_t scaled_axis_data[MAX_AXIS_NUM];
analog_data_t raw_axis_data[MAX_AXIS_NUM];
analog_data_t out_axis_data[MAX_AXIS_NUM];
analog_data_t tmp_axis_data[PREBUF_SIZE][MAX_AXIS_NUM];

analog_data_t FILTER_LEVEL_1_COEF[FILTER_BUF_SIZE] = {40, 30, 15, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
analog_data_t FILTER_LEVEL_2_COEF[FILTER_BUF_SIZE] = {30, 20, 10, 10, 10, 6, 6, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
analog_data_t FILTER_LEVEL_3_COEF[FILTER_BUF_SIZE] = {25, 20, 10, 10, 8, 6, 6, 4, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0};
analog_data_t FILTER_LEVEL_4_COEF[FILTER_BUF_SIZE] = {20, 15, 10, 8, 8, 6, 6, 6, 4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0};
analog_data_t FILTER_LEVEL_5_COEF[FILTER_BUF_SIZE] = {15, 13, 11, 10, 10, 9, 7, 4, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1};
analog_data_t FILTER_LEVEL_6_COEF[FILTER_BUF_SIZE] = {12, 10, 8, 8, 8, 7, 7, 6, 6, 5, 4, 4, 3, 3, 2, 2, 2, 1, 1, 1};
analog_data_t FILTER_LEVEL_7_COEF[FILTER_BUF_SIZE] = {8, 8, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 3, 2, 1, 1, 1};

analog_data_t filter_buffer[MAX_AXIS_NUM][FILTER_BUF_SIZE];
analog_data_t deadband_buffer[MAX_AXIS_NUM][DEADBAND_BUF_SIZE];
	
logical_buttons_state_t axes_buttons[MAX_AXIS_NUM][3];
int32_t	axes_trim_value[MAX_AXIS_NUM];

uint8_t adc_cnt = 0;
uint8_t sensors_cnt = 0;	
	
adc_channel_config_t channel_config[MAX_AXIS_NUM] =
{
	{ADC_Channel_0, 0}, {ADC_Channel_1, 1}, 
	{ADC_Channel_2, 2}, {ADC_Channel_3, 3},
	{ADC_Channel_4, 4}, {ADC_Channel_5, 5}, 
	{ADC_Channel_6, 6}, {ADC_Channel_7, 7}, 
};


unsigned int iabs (int x)
{
	if (x >= 0) return x;
	else return -x;
}

/**
  * @brief  Transform value from input range to value in output range
	*	@param	x: Value to transform
	*	@param	in_min:	Minimum value of input range
	*	@param	in_max:	Maximum value of input range
	*	@param	out_min: Minimum value of output range
	*	@param	out_max: Maximum value of output range
  * @retval Transformed value
  */
static int32_t map2(	int32_t x, 
											int32_t in_min, 
											int32_t in_max, 
											int32_t out_min,
											int32_t out_max)
{
	int32_t tmp;
	int32_t ret;
	
	tmp = x;
	
	
	if (tmp < in_min)	return out_min;
	if (tmp > in_max)	return out_max;
		
	ret = (tmp - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	
	return ret;
}

/**
  * @brief  Transform value of input range -180000 to 180000 to range -32767 to 32767
	*	@param	x: Value to transform
  * @retval Transformed value
  */
static int32_t map_tle (int32_t x)
{
	int32_t tmp;
	int32_t ret;
	
	tmp = x+180;
	
	ret = tmp * 100 / 549 - 32767;
	
	return ret;
}

/**
  * @brief  Transform value from input range to value in output range 
	*	@param	x: Value to transform
	*	@param	in_min:	Minimum value of input range
	*	@param	in_center: Center value of input range
	*	@param	in_max:	Maximum value of input range
	*	@param	out_min: Minimum value of output range
	*	@param	out_center:	Center value of input range
	*	@param	out_max: Maximum value of output range
	*	@param	deadband_size: Width of center dead zone
  * @retval Transformed value
  */
static int32_t map3(	int32_t x, 
											int32_t in_min, 
											int32_t in_center, 
											int32_t in_max, 
											int32_t out_min,
											int32_t out_center,
											int32_t out_max,
											uint8_t deadband_size)
{
	int32_t tmp;
	int32_t ret;
	int32_t dead_zone_right;
	int32_t dead_zone_left;
	
	tmp = x;
	dead_zone_right = ((in_max - in_center)*deadband_size)>>10;
	dead_zone_left = ((in_center - in_min)*deadband_size)>>10;
	
	if (tmp < in_min)	return out_min;
	if (tmp > in_max)	return out_max; 
	if ((tmp > in_center && (tmp - in_center) < dead_zone_right) || 
			(tmp < in_center &&	(in_center - tmp) < dead_zone_left))
	{
		return in_center;
	}		
	
	if (tmp < in_center)
	{
		ret = ((tmp - in_min) * (out_center - out_min) / (in_center - dead_zone_left - in_min) + out_min);
  }
	else
	{
		ret = ((tmp - in_center - dead_zone_right) * (out_max - out_center) / (in_max - in_center - dead_zone_right) + out_center);
	}
	return ret;
}

/**
  * @brief  Lowing input data resolution
	*	@param	value: Value to process
	*	@param	resolution:	Desired resolution of value in bits
  * @retval Resulting value
  */
analog_data_t SetResolutioin (analog_data_t value, uint8_t resolution)
{
	int32_t tmp = 0;
	int32_t ret = 0;
	uint32_t fullscale = AXIS_MAX_VALUE - AXIS_MIN_VALUE;
	float step;
	
	if (resolution >= 16)
	{
		return value;
	}
	else if (resolution > 0)
	{
		tmp = fullscale >> (16 - resolution);
		step = (float)fullscale/tmp;
		
		tmp = value;
		tmp = (tmp - AXIS_MIN_VALUE)  >> (16 - resolution);
		ret = step * tmp + AXIS_MIN_VALUE;
	}
	
	return ret;
}

/**
  * @brief  FIR filter for input data
	*	@param	value: Value to process
	*	@param	filter_buf:	Pointer to filter data buffer
	*	@param	filter_lvl:	Desired filter level
	*   This parameter can be 0-3 (where 0 is no filtration, 3 is high filtration level)
  * @retval Resulting value
  */
analog_data_t Filter (analog_data_t value, analog_data_t * filter_buf, filter_t filter_lvl)
{
	int32_t tmp32;
	
	switch (filter_lvl)
	{
		default:
		case FILTER_NO:
			return value;
		
		case FILTER_LEVEL_1:
			tmp32 = value * FILTER_LEVEL_1_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_1_COEF[i];
			}
		break;
		
		case FILTER_LEVEL_2:
			tmp32 = value * FILTER_LEVEL_2_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_2_COEF[i];
			}
		break;
		
		case FILTER_LEVEL_3:
			tmp32 = value * FILTER_LEVEL_3_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_3_COEF[i];
			}
		break;
			
		case FILTER_LEVEL_4:
			
			tmp32 = value * FILTER_LEVEL_4_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_4_COEF[i];
			}			
		break;
			
		case FILTER_LEVEL_5:
			
			tmp32 = value * FILTER_LEVEL_5_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_5_COEF[i];
			}			
		break;

		case FILTER_LEVEL_6:
			
			tmp32 = value * FILTER_LEVEL_6_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_6_COEF[i];
			}			
		break;
			
		case FILTER_LEVEL_7:
			
			tmp32 = value * FILTER_LEVEL_7_COEF[0];
			for (uint8_t i=FILTER_BUF_SIZE-1; i>0; i--)
			{
				filter_buf[i] = filter_buf[i-1];
				
				tmp32 += filter_buf[i] * FILTER_LEVEL_7_COEF[i];
			}			
		break;
	}
	
	filter_buf[0] = (uint16_t)(tmp32/100);
	
	
	return filter_buf[0];
}

/**
  * @brief  Dynamic deadband implementation
	*	@param	value: Value to process
	*	@param	deadband_buf:	Pointer to deadband data buffer
	* @param	deadband_size: Deadband width
  * @retval Is holding?
  */
uint8_t IsDynamicDeadbandHolding (analog_data_t value, analog_data_t * deadband_buf, uint8_t deadband_size)
{
	uint8_t is_holding = 1;
	int32_t	disp = 0;
	int32_t mean = 0;
	int32_t treshold;
	
	treshold = 3 * deadband_size;
	
	// calculating mean value
	for (uint8_t i=0; i<DEADBAND_BUF_SIZE; i++)
	{
		mean += deadband_buf[i];
	}
	mean = mean/DEADBAND_BUF_SIZE;
	
	// calculating dispercy
	for (uint8_t i=0; i<DEADBAND_BUF_SIZE; i++)
	{
		disp += (deadband_buf[i] - mean)*(deadband_buf[i] - mean);
	}
	disp = disp/DEADBAND_BUF_SIZE;
	
	if (disp > treshold*treshold)	
	{
		is_holding = 0;
	}
	// shift buffer data and add a new value
	for (uint8_t i=DEADBAND_BUF_SIZE-1; i>0; i--)
	{
		deadband_buf[i] = deadband_buf[i-1];
	}
	deadband_buf[0] = value;
	
	return is_holding;
}

/**
  * @brief  Scaling input data accodring to set axis curve shape
	*	@param	p_axis_cfg: Pointer to axis configuration structure
	*	@param	value:	Value to process
	*	@param	point_cnt:	Number of points in axis curve
  * @retval Resulting value
  */
analog_data_t ShapeFunc (axis_config_t * p_axis_cfg,  analog_data_t value, uint8_t point_cnt)
{
	int32_t out_min, out_max, step;
	int32_t in_min, in_max;
	uint8_t min_index;
	analog_data_t ret;
	
	// check if is not linear
	if (p_axis_cfg->curve_shape[0] != -100 ||  
			p_axis_cfg->curve_shape[1] != -80 ||
			p_axis_cfg->curve_shape[2] != -60 ||
			p_axis_cfg->curve_shape[3] != -40 ||
			p_axis_cfg->curve_shape[4] != - 20 ||
			p_axis_cfg->curve_shape[5] != 0 || 
			p_axis_cfg->curve_shape[6] != 20 ||
			p_axis_cfg->curve_shape[7] != 40 ||
			p_axis_cfg->curve_shape[8] != 60 ||
			p_axis_cfg->curve_shape[9] != 80 ||
			p_axis_cfg->curve_shape[10] != 100)
	{	
		int32_t tmp = value - AXIS_MIN_VALUE;
		int32_t fullscale = AXIS_MAX_VALUE - AXIS_MIN_VALUE;
		
		step = (float)fullscale/((float)point_cnt-1.0f);
		min_index = tmp/step;
		
		if (min_index == point_cnt-1) min_index = point_cnt-2;
		
		in_min = AXIS_MIN_VALUE + min_index*step;
		in_max = AXIS_MIN_VALUE + (min_index+1)*step;
		
		out_min = ((int32_t)p_axis_cfg->curve_shape[min_index] * (int32_t)fullscale/200 + (int32_t)AXIS_CENTER_VALUE);
		out_max = ((int32_t)p_axis_cfg->curve_shape[min_index+1] * (int32_t)fullscale/200 + (int32_t)AXIS_CENTER_VALUE);
		
		ret = map2(value, in_min, in_max, out_min, out_max);
	}
	else
	{
		ret = value;
	}
	
	return(ret);
}

/**
  * @brief  Axes initialization after startup
	*	@param	p_dev_config: Pointer to device configuration structure
  * @retval None
  */
void AxesInit (dev_config_t * p_dev_config)
{
  ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	 /* DMA and ADC controller clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
	for (int i = 0; i<MAX_AXIS_NUM; i++)
	{
		sensors[i].source = -1;
		sensors[i].rx_complete = 1;
		sensors[i].tx_complete = 1;
	}
	
	// Init sources
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == AXIS_ANALOG)
		{
			adc_cnt++;
		}
		else if (p_dev_config->pins[i] == TLE5011_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = TLE5011;			
					sensors[sensors_cnt].source = i;
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[i] == MCP3201_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = MCP3201;			
					sensors[sensors_cnt].source = i;
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[i] == MCP3202_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = MCP3202;	
					sensors[sensors_cnt].source = i;
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[i] == MCP3204_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = MCP3204;				
					sensors[sensors_cnt].source = i;
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[i] == MCP3208_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = MCP3208;	
					sensors[sensors_cnt].source = i;
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[i] == MLX90393_CS)
		{
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
			{
				if (p_dev_config->axis_config[k].source_main == i && sensors_cnt < MAX_AXIS_NUM)
				{
					sensors[sensors_cnt].type = MLX90393_SPI;
					sensors[sensors_cnt].source = i;
					
					MLX90393_Start(&sensors[sensors_cnt]);
					sensors_cnt++;
					break;
				}
			}
		}
		else if (p_dev_config->pins[19] == I2C_SCL && p_dev_config->pins[20] == I2C_SDA)			// PB8 and PB9
		{
			// look for ADS1115 sensors with different addresses
			for (uint8_t addr = ADS1115_I2C_ADDR_MIN; addr <= ADS1115_I2C_ADDR_MAX; addr ++)
			{
				for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
				{
					if (p_dev_config->axis_config[k].source_main == (pin_t) SOURCE_I2C)
					{
						if ((p_dev_config->axis_config[k].i2c_address) == addr)
						{
							sensors[sensors_cnt].address = p_dev_config->axis_config[k].i2c_address;
							sensors[sensors_cnt].type = ADS1115;
							sensors[sensors_cnt].source = (pin_t) SOURCE_I2C;
							
							ADS1115_Init(&sensors[sensors_cnt]);
							sensors_cnt++;
							break;
						}
					}
				}
			}
			// look for AS5600
			for (uint8_t k=0; k<MAX_AXIS_NUM; k++)
				{
					if (p_dev_config->axis_config[k].source_main == (pin_t) SOURCE_I2C)
					{
						if ((p_dev_config->axis_config[k].i2c_address) == AS5600_I2C_ADDR)
						{
							sensors[sensors_cnt].address = p_dev_config->axis_config[k].i2c_address;
							sensors[sensors_cnt].type = AS5600;
							sensors[sensors_cnt].source = (pin_t) SOURCE_I2C;
							
							uint16_t calib_min = map2(p_dev_config->axis_config[k].calib_min, AXIS_MIN_VALUE, AXIS_MAX_VALUE, 0, 4095);
							uint16_t calib_max = map2(p_dev_config->axis_config[k].calib_max, AXIS_MIN_VALUE, AXIS_MAX_VALUE, 0, 4095);
							
							AS5600_Init(&sensors[sensors_cnt], calib_min, calib_max);						
							sensors_cnt++;						
							break;
						}
					}
				}
		}
	}
	
	// Init ADC
	if (adc_cnt > 0)
	{
		/* ADC1 configuration ------------------------------------------------------*/
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = MAX_AXIS_NUM;
		ADC_Init(ADC1, &ADC_InitStructure);

		/* Enable ADC1 DMA */
		ADC_DMACmd(ADC1, ENABLE);
	}
	
	uint8_t axis_num = 0;
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		// Configure Sensors channels		
		if (p_dev_config->pins[i] == TLE5011_CS)
		{
			axis_num++;
		}
	}
	for (int i=0; i<MAX_AXIS_NUM; i++)
	{ 
		if (p_dev_config->pins[i] == AXIS_ANALOG)		// Configure ADC channels
		{
			/* ADC1 regular channel configuration */ 
			ADC_RegularChannelConfig(ADC1, channel_config[i].channel, i+1, ADC_SampleTime_239Cycles5);
			axis_num++;
		}
		
	}

	if (adc_cnt > 0)
	{
		/* DMA1 channel1 configuration ----------------------------------------------*/
		DMA_DeInit(DMA1_Channel1);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &input_data[0];
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = MAX_AXIS_NUM;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
			/* Enable ADC1 */
		ADC_Cmd(ADC1, ENABLE);
			
			/* Enable ADC1 reset calibration register */   
		ADC_ResetCalibration(ADC1);
		/* Check the end of ADC1 reset calibration register */
		while(ADC_GetResetCalibrationStatus(ADC1));

		/* Start ADC1 calibration */
		ADC_StartCalibration(ADC1);
		/* Check the end of ADC1 calibration */
		while(ADC_GetCalibrationStatus(ADC1));
	}
}

/**
  * @brief  ADC conversion processing routine
  * @retval None
  */
void ADC_Conversion (void)
{
	SEGGER_SYSVIEW_RecordVoid(45);
	
	uint8_t num_of_conv = 0;
	analog_data_t tmp = 0;
	
	if (adc_cnt > 0)
	{
		for (uint8_t i=0; i<PREBUF_SIZE; i++)	
		{
			DMA1_Channel1->CMAR = (uint32_t) &tmp_axis_data[num_of_conv++];
			DMA_SetCurrDataCounter(DMA1_Channel1, MAX_AXIS_NUM);	
			DMA_Cmd(DMA1_Channel1, ENABLE);
			ADC_Cmd(ADC1, ENABLE);
			/* Start ADC1 Software Conversion */ 
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);

			while (!DMA_GetFlagStatus(DMA1_FLAG_TC1));
			DMA_ClearFlag(DMA1_FLAG_TC1);
				
			ADC_Cmd(ADC1, DISABLE);
			DMA_Cmd(DMA1_Channel1, DISABLE);
			
			if (num_of_conv > PREBUF_SIZE - 1) 
			{
				num_of_conv = 0;
				for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
				{
					tmp = 0;
					for (uint8_t k=0; k<PREBUF_SIZE; k++)
					{
						tmp += tmp_axis_data[k][i];					
					}
					input_data[i] = tmp/PREBUF_SIZE;
				}
			}
		}
	}
	SEGGER_SYSVIEW_RecordEndCall(45);
}

/**
  * @brief  Axes data processing routine
	*	@param	p_dev_config: Pointer to device configuration structure
  * @retval None
  */
void AxesProcess (dev_config_t * p_dev_config)
{
	SEGGER_SYSVIEW_RecordVoid(46);
	
	int32_t tmp[MAX_AXIS_NUM];
	float tmpf;
	
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		
		int8_t source = p_dev_config->axis_config[i].source_main;
		uint8_t channel = p_dev_config->axis_config[i].channel;
		uint8_t address = p_dev_config->axis_config[i].i2c_address;
		
		if (source >= 0)		// source SPI sensors or internal ADC
		{
			if (p_dev_config->pins[source] == AXIS_ANALOG)					// source analog
			{
				if (p_dev_config->axis_config[i].offset_angle > 0)
				{
						tmp[i] = input_data[source] - p_dev_config->axis_config[i].offset_angle * 170;
						if (tmp[i] < 0) tmp[i] += 4095;
						else if (tmp[i] > 4095) tmp[i] -= 4095;
				}
				else
				{
					tmp[i] = input_data[source];
				}
				
				raw_axis_data[i] = map2(tmp[i], 0, 4095, AXIS_MIN_VALUE, AXIS_MAX_VALUE);
			}
			else if (p_dev_config->pins[source] == TLE5011_CS)			// source TLE5011
			{
				tmpf = 0;
				uint8_t k=0;
				// search for needed sensor
				for (k=0; k<MAX_AXIS_NUM; k++)
				{
					if (sensors[k].source == source) break;
				}
				// get angle data
				if (TLE501x_GetAngle(&sensors[k], &tmpf) == 0)
				{
					sensors[k].ok_cnt++;
					if (p_dev_config->axis_config[i].offset_angle > 0)
					{
						tmpf -= p_dev_config->axis_config[i].offset_angle * 15;
						if (tmpf < -180) tmpf += 360;
						else if (tmpf > 180) tmpf -= 360;
					}
					tmpf *= 1000;
					raw_axis_data[i] = map_tle(tmpf);
				}
				else
				{
					sensors[k].err_cnt++;
				}
			}
			else if (p_dev_config->pins[source] == MCP3201_CS)				// source MCP3201
			{
				uint8_t k=0;
				// search for needed sensor
				for (k=0; k<MAX_AXIS_NUM; k++)
				{
					if (sensors[k].source == source) break;
				}
				// get data
				if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
				{
					tmp[i] = MCP320x_GetData(&sensors[k], 0) - p_dev_config->axis_config[i].offset_angle * 170;
					if (tmp[i] < 0) tmp[i] += 4095;
					else if (tmp[i] > 4095) tmp[i] -= 4095;
				}
				else		// offset disabled
				{
					tmp[i] = MCP320x_GetData(&sensors[k], 0);
				}
				
				raw_axis_data[i] = map2(tmp[i], 0, 4095, AXIS_MIN_VALUE, AXIS_MAX_VALUE);
			}	
			else if (p_dev_config->pins[source] == MCP3202_CS ||
							 p_dev_config->pins[source] == MCP3204_CS ||
							 p_dev_config->pins[source] == MCP3208_CS)				// source MCP3202/4/8
			{
				uint8_t k=0;
				// search for needed sensor
				for (k=0; k<MAX_AXIS_NUM; k++)
				{
					if (sensors[k].source == source) break;
				}
				// get data
				if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
				{
					tmp[i] = MCP320x_GetData(&sensors[k], channel) - p_dev_config->axis_config[i].offset_angle * 170;
					if (tmp[i] < 0) tmp[i] += 4095;
					else if (tmp[i] > 4095) tmp[i] -= 4095;
				}
				else		// offset disabled
				{
					tmp[i] = MCP320x_GetData(&sensors[k], channel);
				}
				
				raw_axis_data[i] = map2(tmp[i], 0, 4095, AXIS_MIN_VALUE, AXIS_MAX_VALUE);
			}	
			else if (p_dev_config->pins[source] == MLX90393_CS)				// source MLX90393
			{
				uint8_t k = 0;
				uint16_t tmp16 = 0;
				// search for needed sensor
				for (k=0; k<MAX_AXIS_NUM; k++)
				{
					if (sensors[k].source == source) break;
				}
				if (MLX90393_GetData(&tmp16, &sensors[k], channel) == 0)
				{
					sensors[k].ok_cnt++;
					
					if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
					{
						raw_axis_data[i] = tmp16 - p_dev_config->axis_config[i].offset_angle * 2730;
						if (raw_axis_data[i] < AXIS_MIN_VALUE) raw_axis_data[i] += AXIS_FULLSCALE;
						else if (raw_axis_data[i] > AXIS_MAX_VALUE) raw_axis_data[i] -= AXIS_FULLSCALE;
					}
					else
					{
						raw_axis_data[i] = tmp16;
					}
				}
				else
				{
					sensors[k].err_cnt++;
				}
			}	
		}
		else if (source == (axis_source_t)SOURCE_I2C)				// source I2C sensor
		{
			uint8_t k=0;
			// search for needed sensor
			for (k=0; k<MAX_AXIS_NUM; k++)
			{
				if (sensors[k].address == address) break;
			}
			// get data
			if (sensors[k].type == ADS1115)
			{					
				if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
				{
					tmp[i] = ADS1115_GetData(&sensors[k], channel) - p_dev_config->axis_config[i].offset_angle * 2730;
					if (tmp[i] < 0) tmp[i] += 32767;
					else if (tmp[i] > 32767) tmp[i] -= 32767;
				}
				else		// offset disabled
				{
					tmp[i] = ADS1115_GetData(&sensors[k], channel);
				}
				raw_axis_data[i] = map2(tmp[i], 0, 32767, AXIS_MIN_VALUE, AXIS_MAX_VALUE);
			}
			else if (sensors[k].type == AS5600)
			{
				if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
				{
					tmp[i] = AS5600_GetScaledData(&sensors[k]) - p_dev_config->axis_config[i].offset_angle * 170;
					if (tmp[i] < 0) tmp[i] += 4095;
					else if (tmp[i] > 4095) tmp[i] -= 4095;
				}
				else		// offset disabled
				{
					tmp[i] = AS5600_GetScaledData(&sensors[k]);
				}					
				raw_axis_data[i] = map2(tmp[i], 0, 4095, AXIS_MIN_VALUE, AXIS_MAX_VALUE);
			}
		}				
		
		else if (source == SOURCE_ENCODER)		// source encoder
		{
			uint8_t encoder_num = p_dev_config->axis_config[i].channel;
			
			if (encoders_state[encoder_num].cnt > AXIS_MAX_VALUE) encoders_state[encoder_num].cnt = AXIS_MAX_VALUE;
			if (encoders_state[encoder_num].cnt < AXIS_MIN_VALUE) encoders_state[encoder_num].cnt = AXIS_MIN_VALUE;
			
			if (p_dev_config->axis_config[i].offset_angle > 0)	// offset enabled
			{
				tmp[i] = encoders_state[p_dev_config->axis_config[i].channel].cnt - p_dev_config->axis_config[i].offset_angle * 170;
				if (tmp[i] < 0) tmp[i] += 32767;
				else if (tmp[i] > 32767) tmp[i] -= 32767;
			}
			else		// offset disabled
			{	
				tmp[i] = encoders_state[p_dev_config->axis_config[i].channel].cnt;
			}
			
			raw_axis_data[i] = tmp[i];
		}
		
		// Filtering
		tmp[i] = Filter(raw_axis_data[i], filter_buffer[i], p_dev_config->axis_config[i].filter);
			
		// Deadband processing and scaling		
		if (!p_dev_config->axis_config[i].is_dynamic_deadband)
		{
			// Scale output data
			tmp[i] = map3( tmp[i], 
									 p_dev_config->axis_config[i].calib_min,
									 p_dev_config->axis_config[i].calib_center,    
									 p_dev_config->axis_config[i].calib_max, 
									 AXIS_MIN_VALUE,
									 AXIS_CENTER_VALUE,
									 AXIS_MAX_VALUE,
									 p_dev_config->axis_config[i].deadband_size); 

			// Shaping
			tmp[i] = ShapeFunc(&p_dev_config->axis_config[i], tmp[i], 11);
			// Lowing resolution if needed
			tmp[i] = SetResolutioin(tmp[i], p_dev_config->axis_config[i].resolution + 1);
			
			// Invertion
			if (p_dev_config->axis_config[i].inverted > 0)
			{
				tmp[i] = 0 - tmp[i];
			}			
		}
		else if (p_dev_config->axis_config[i].is_dynamic_deadband && 
						 iabs(tmp[i] - raw_axis_data[i]) < 3*3*p_dev_config->axis_config[i].deadband_size &&			// 3*3*deadband_size = 3 sigma
						  IsDynamicDeadbandHolding(tmp[i], deadband_buffer[i], p_dev_config->axis_config[i].deadband_size))
		{
			tmp[i] = scaled_axis_data[i];
		}	
		else
		{
			// Scale output data
			tmp[i] = map3( tmp[i],
									 p_dev_config->axis_config[i].calib_min,
									 p_dev_config->axis_config[i].calib_center,    
									 p_dev_config->axis_config[i].calib_max, 
									 AXIS_MIN_VALUE,
									 AXIS_CENTER_VALUE,
									 AXIS_MAX_VALUE,
									 0); 
			// Shaping
			tmp[i] = ShapeFunc(&p_dev_config->axis_config[i], tmp[i], 11);
			// Lowing resolution if needed
			tmp[i] = SetResolutioin(tmp[i], p_dev_config->axis_config[i].resolution + 1);
			
			// Invertion
			if (p_dev_config->axis_config[i].inverted > 0)
			{
				tmp[i] = 0 - tmp[i];
			}
		}
		
		// Prescaling
		if (p_dev_config->axis_config[i].prescaler != 100 &&  
			 ((axes_buttons[i][0].current_state && p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_PRESCALER_EN) ||
			 (axes_buttons[i][1].current_state && p_dev_config->axis_config[i].button2_type == AXIS_BUTTON_PRESCALER_EN) ||
		   (axes_buttons[i][2].current_state && p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_PRESCALER_EN)) )
		{
			tmp[i] = tmp[i]  * p_dev_config->axis_config[i].prescaler / 100;
		}
		
		// Buttons section
    {
			int64_t millis = GetTick();
			
			uint8_t inc_button_num = 0;
			uint8_t rst_button_num = 0;
			uint8_t dec_button_num = 0;
			uint8_t cent_button_num = 0;
			
			// detect buttons
			if (p_dev_config->axis_config[i].button1 >= 0 && 
					p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_UP)	inc_button_num |= 1<<0;
			if (p_dev_config->axis_config[i].button3 >= 0 && 
							 p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_UP)	inc_button_num |= 1<<2;
			
			if (p_dev_config->axis_config[i].button1 >= 0 && 
					p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_RESET)	rst_button_num |= 1<<0;
			if (p_dev_config->axis_config[i].button2 >= 0 && 
							 p_dev_config->axis_config[i].button2_type == AXIS_BUTTON_RESET)	rst_button_num |= 1<<1;
			if (p_dev_config->axis_config[i].button3 >= 0 && 
							 p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_RESET)	rst_button_num |= 1<<2;
			
			if (p_dev_config->axis_config[i].button1 >= 0 && 
					p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_DOWN)	dec_button_num |= 1<<0;
			if (p_dev_config->axis_config[i].button3 >= 0 && 
							 p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_DOWN)	dec_button_num |= 1<<2;
			
			if (p_dev_config->axis_config[i].button1 >= 0 && 
					p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_CENTER)	cent_button_num |= 1<<0;
			if (p_dev_config->axis_config[i].button2 >= 0 && 
							 p_dev_config->axis_config[i].button2_type == AXIS_BUTTON_CENTER)	cent_button_num |= 1<<1;
			if (p_dev_config->axis_config[i].button3 >= 0 && 
							 p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_CENTER)	cent_button_num |= 1<<2;
			
			axes_buttons[i][0].prev_physical_state = axes_buttons[i][0].current_state;
			axes_buttons[i][1].prev_physical_state = axes_buttons[i][1].current_state;
			axes_buttons[i][2].prev_physical_state = axes_buttons[i][2].current_state;

			// get new button's states
			if (p_dev_config->axis_config[i].button1 >= 0)
			{
				axes_buttons[i][0].current_state = logical_buttons_state[p_dev_config->axis_config[i].button1].current_state;
			}
			if (p_dev_config->axis_config[i].button2 >= 0)
			{
				axes_buttons[i][1].current_state = logical_buttons_state[p_dev_config->axis_config[i].button2].current_state;
			}
			if (p_dev_config->axis_config[i].button3 >= 0)
			{
				axes_buttons[i][2].current_state = logical_buttons_state[p_dev_config->axis_config[i].button3].current_state;
			}	
			
			// Trimming by buttons
			if (inc_button_num > 0 || rst_button_num > 0 || dec_button_num > 0)
			{
				// increment
				if ((inc_button_num & 0x01) && axes_buttons[i][0].current_state > axes_buttons[i][0].prev_physical_state)
				{
					axes_buttons[i][0].time_last = millis + 500;
					axes_trim_value[i] +=  (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((inc_button_num & 0x01) && axes_buttons[i][0].prev_physical_state && millis - axes_buttons[i][0].time_last > 50)
				{
					axes_buttons[i][0].time_last = millis;
					axes_trim_value[i] += (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((inc_button_num & 0x04) && axes_buttons[i][2].current_state > axes_buttons[i][2].prev_physical_state)
				{
					axes_buttons[i][2].time_last = millis + 500;
					axes_trim_value[i] +=  (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((inc_button_num & 0x04) && axes_buttons[i][2].prev_physical_state && millis - axes_buttons[i][2].time_last > 50)
				{
					axes_buttons[i][2].time_last = millis;
					axes_trim_value[i] += (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
						
				// decrement
				if ((dec_button_num & 0x01) && axes_buttons[i][0].current_state > axes_buttons[i][0].prev_physical_state)
				{
					axes_buttons[i][0].time_last = millis + 500;
					axes_trim_value[i] -=  (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((dec_button_num & 0x01) && axes_buttons[i][0].prev_physical_state && millis - axes_buttons[i][0].time_last > 50)
				{
					axes_buttons[i][0].time_last = millis;
					axes_trim_value[i] -= (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((dec_button_num & 0x04) && axes_buttons[i][2].current_state > axes_buttons[i][2].prev_physical_state)
				{
					axes_buttons[i][2].time_last = millis + 500;
					axes_trim_value[i] -=  (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				else if ((dec_button_num & 0x04) && axes_buttons[i][2].prev_physical_state && millis - axes_buttons[i][2].time_last > 50)
				{
					axes_buttons[i][2].time_last = millis;
					axes_trim_value[i] -= (AXIS_FULLSCALE>>1)/p_dev_config->axis_config[i].divider;
				}
				
				// reset
				if ((rst_button_num & 0x01) && axes_buttons[i][0].current_state)
				{
					axes_trim_value[i] = 0;
				}
				else if ((rst_button_num & 0x02) && axes_buttons[i][1].current_state)
				{
					axes_trim_value[i] = 0;
				}
				else if ((rst_button_num & 0x04) && axes_buttons[i][2].current_state)
				{
					axes_trim_value[i] = 0;
				}	
			}

			// Helicopter-type trimming (centering)
			if (cent_button_num > 0)
			{
				if ((cent_button_num & 0x01) && axes_buttons[i][0].current_state)
				{
					axes_trim_value[i] = -tmp[i];
				}
				else if ((cent_button_num & 0x02) && axes_buttons[i][1].current_state)
				{
					axes_trim_value[i] = -tmp[i];
				}
				else if ((cent_button_num & 0x04) && axes_buttons[i][2].current_state)
				{
					axes_trim_value[i] = -tmp[i];
				}
			}
			
			if (axes_trim_value[i] > AXIS_MAX_VALUE) axes_trim_value[i] = AXIS_MAX_VALUE;
			if (axes_trim_value[i] < AXIS_MIN_VALUE) axes_trim_value[i] = AXIS_MIN_VALUE;
			
			tmp[i] += axes_trim_value[i];
			
			if (tmp[i] > AXIS_MAX_VALUE) tmp[i] = AXIS_MAX_VALUE;
			if (tmp[i] < AXIS_MIN_VALUE) tmp[i] = AXIS_MIN_VALUE;
    }
		
	} 
	
	// Multi-axis process
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		
		// check axis function activation 
		if(((axes_buttons[i][0].current_state && p_dev_config->axis_config[i].button1_type == AXIS_BUTTON_FUNC_EN) ||
			 (axes_buttons[i][1].current_state && p_dev_config->axis_config[i].button2_type == AXIS_BUTTON_FUNC_EN) ||
		   (axes_buttons[i][2].current_state && p_dev_config->axis_config[i].button3_type == AXIS_BUTTON_FUNC_EN) ||
		   (p_dev_config->axis_config[i].button1_type != AXIS_BUTTON_FUNC_EN && 
				p_dev_config->axis_config[i].button2_type != AXIS_BUTTON_FUNC_EN && 
				p_dev_config->axis_config[i].button3_type != AXIS_BUTTON_FUNC_EN)) && 
				p_dev_config->axis_config[i].function != NO_FUNCTION)
		{
			{
				switch (p_dev_config->axis_config[i].function)
				{
					case FUNCTION_PLUS_ABS:
						tmp[i] = tmp[i]/2 + tmp[p_dev_config->axis_config[i].source_secondary]/2;
						break;
					case FUNCTION_PLUS_REL:
						tmp[i] = tmp[i] + tmp[p_dev_config->axis_config[i].source_secondary] - AXIS_MIN_VALUE;
						break;
					case FUNCTION_MINUS_ABS:
						tmp[i] = tmp[i]/2 - tmp[p_dev_config->axis_config[i].source_secondary]/2;
						break;
					case FUNCTION_MINUS_REL:
						tmp[i] = tmp[i] - tmp[p_dev_config->axis_config[i].source_secondary] + AXIS_MIN_VALUE;
						break;
					case FUNCTION_EQUAL:
						tmp[i] = tmp[p_dev_config->axis_config[i].source_secondary];
						break;
					default:
						break;
				}
			}
			if (tmp[i] > AXIS_MAX_VALUE) tmp[i] = AXIS_MAX_VALUE;
			else if (tmp[i] < AXIS_MIN_VALUE) tmp[i] = AXIS_MIN_VALUE;
		}
		
		// disable data updating from IRQ
		NVIC_DisableIRQ(TIM2_IRQn);
		
    // setting technical axis data
    scaled_axis_data[i] = tmp[i];
    // setting output axis data
    if (p_dev_config->axis_config[i].out_enabled)  out_axis_data[i] = scaled_axis_data[i];
    else  out_axis_data[i] = 0;
		
		// restore IRQ
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	
	SEGGER_SYSVIEW_RecordEndCall(46);
}

/**
  * @brief  Resetting axis calibration values to the default
	*	@param	p_dev_config: Pointer to device configuration structure
	*	@param	axis_num: Number of axis 
  * @retval None
  */
void AxisResetCalibration (dev_config_t * p_dev_config, uint8_t axis_num)
{
	p_dev_config->axis_config[axis_num].calib_max = AXIS_MIN_VALUE;
	p_dev_config->axis_config[axis_num].calib_center = AXIS_CENTER_VALUE;
	p_dev_config->axis_config[axis_num].calib_min = AXIS_MAX_VALUE;
}

/**
  * @brief  Getting axes data in report format
	*	@param	out_data: Pointer to target buffer of axes output data (may be disabled in configuration)
	*	@param	scaled_data: Pointer to target buffer of axes scaled output data
	*	@param	raw_data: Pointer to target buffer of axes raw output data 
  * @retval None
  */
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




