/**
  ******************************************************************************
  * @file           : tle5011.c
  * @brief          : TLE5011 sensors driver implementation
			
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

#include "tle5011.h"
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
	
	SPI_HalfDuplex_Transmit(&cmd, 1, TLE5011_SPI_MODE);
	if (length > 0)
	{
		SPI_HalfDuplex_Receive(data, length+1, TLE5011_SPI_MODE);
	}

}

void TLE501x_Write(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = addr<<3 | (addr & 0x0F)<<3 | (length & 0x07);
	SPI_HalfDuplex_Transmit(&cmd, 1, TLE5011_SPI_MODE);
	if (length > 0)
	{
		SPI_HalfDuplex_Transmit(data, length, TLE5011_SPI_MODE);
	}
}

int TLE501x_GetAngle(sensor_t * sensor, float * angle)
{
	int16_t x_value, y_value;
	float out = 0;
	int ret = 0;
	
	if (CheckCrc(&sensor->data[1], sensor->data[5], 0xFB, 4))
	{
		x_value = sensor->data[2]<<8 | sensor->data[1];
		y_value = sensor->data[4]<<8 | sensor->data[3];
		
		
		out = atan2f((float)y_value, (float)x_value)/ M_PI * (float)180.0;			
		*angle = out;
		ret = 0;
	}
	else
	{
		ret = -1;
	}
	
	
	return ret;
}

void TLE501x_StartDMA(sensor_t * sensor)
{	
	sensor->rx_complete = 1;
	sensor->tx_complete = 0;
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	sensor->data[0] = 0x00;
	sensor->data[1] = 0x8C;
	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	
	SPI_HalfDuplex_Transmit(&sensor->data[0], 2, TLE5011_SPI_MODE);
}

void TLE501x_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}



