/**
  ******************************************************************************
  * @file           : sensors.c
  * @brief          : Sensors driver implementation
  ******************************************************************************
  */

#include "sensors.h"
#include <math.h>

uint8_t MathCRC8(uint8_t crc, uint8_t data)
{
	crc ^= data;
	for (uint8_t bit=0 ; bit<8 ; bit++ ) 
	{ 
		if ((crc & 0x80)!=0) 
		{ 
			crc <<= 1; 
			crc ^= 0x1D; 
		} else 
		{ 
			crc <<= 1; 
		}; 
	};
	return(crc);
};

int CheckCrc(uint8_t * data, uint8_t crc, uint8_t initial, uint8_t length) 
{
  uint8_t ret = initial;
	uint8_t index = 0;
	
	while (index < length)
	{
		ret = MathCRC8(ret, data[index++]);
	}        
  ret = ~ret;
	
  return (ret == crc);
}

void TLE501x_Read(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = 0x80 | (addr & 0x0F)<<3 | (length & 0x07);
	
	HAL_SPI_Transmit(&hspi1, &cmd, 1, TLE501x_TIMEOUT);
	if (length > 0)
	{
		HAL_SPI_Receive(&hspi1, data, length+1, TLE501x_TIMEOUT);
	}

}

void TLE501x_Write(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = addr<<3 | (addr & 0x0F)<<3 | (length & 0x07);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, TLE501x_TIMEOUT);
	if (length > 0)
	{
		HAL_SPI_Transmit(&hspi1, data, length, TLE501x_TIMEOUT);
	}
}

int TLE501x_Get(pin_config_t * p_cs_pin_config, float * data)
{
	uint8_t tmp_buf[6];
	int16_t x_value, y_value;
	float angle;
	
	
	// Update command
	tmp_buf[0] = 0x00;
	HAL_GPIO_WritePin(p_cs_pin_config->port, p_cs_pin_config->pin, GPIO_PIN_RESET);		
	TLE501x_Write(&tmp_buf[0], 0x00, 0);	
	
	// Get sensor data	
	TLE501x_Read(&tmp_buf[1], 0x01, 4);	
	HAL_GPIO_WritePin(p_cs_pin_config->port, p_cs_pin_config->pin, GPIO_PIN_SET);
	
	if (CheckCrc(&tmp_buf[1], tmp_buf[5], 0xFB, 4))
	{
		x_value = tmp_buf[2]<<8 | tmp_buf[1];
		y_value = tmp_buf[4]<<8 | tmp_buf[3];
		
		angle = atan2(y_value, x_value)/ PI * 180;
		
		
		*data = angle;
		return 0;
	}
	else
	{
		return -1;
	}
	
}




