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
	
	SoftSPI_HalfDuplex_Transmit(&cmd, 1);
	if (length > 0)
	{
		SoftSPI_HalfDuplex_Receive(data, length+1);
	}

}

void TLE501x_Write(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = addr<<3 | (addr & 0x0F)<<3 | (length & 0x07);
	SoftSPI_HalfDuplex_Transmit(&cmd, 1);
	if (length > 0)
	{
		SoftSPI_HalfDuplex_Transmit(data, length);
	}
}

int TLE501x_Get(pin_config_t * p_cs_pin_config, double * data)
{
	uint8_t tmp_buf[6];
	int16_t x_value, y_value;
	double angle;
	
	
	// Update command
	tmp_buf[0] = 0x00;
	GPIO_WriteBit(p_cs_pin_config->port, p_cs_pin_config->pin, Bit_RESET);		
	TLE501x_Write(&tmp_buf[0], 0x00, 0);	
	
	// Get sensor data	
	TLE501x_Read(&tmp_buf[1], 0x01, 4);	
	GPIO_WriteBit(p_cs_pin_config->port, p_cs_pin_config->pin, Bit_SET);
	
	if (CheckCrc(&tmp_buf[1], tmp_buf[5], 0xFB, 4))
	{
		x_value = tmp_buf[2]<<8 | tmp_buf[1];
		y_value = tmp_buf[4]<<8 | tmp_buf[3];
		
		angle = atan2((double)y_value, (double)x_value)/ M_PI * (double)180.0;
		
		
		*data = angle;
		return 0;
	}
	else
	{
		return -1;
	}
	
}




