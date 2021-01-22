/**
  ******************************************************************************
  * @file           : mcp320x.c
  * @brief          : MCP320x ADC driver implementation
			
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

#include "mcp320x.h"

uint16_t MCP320x_GetData(sensor_t * sensor, uint8_t channel)
{
	uint16_t ret;
	
	if (sensor->type == MCP3201)
	{
		ret = ((sensor->data[1] & 0x1F)<<7) | (sensor->data[2]>>1);
	}
	else
	{
		ret = (sensor->data[1 + 3*channel] & 0x0F)<<8 | sensor->data[2 + 3*channel];
	}
	return ret;
}

void MCP320x_StartDMA(sensor_t * sensor, uint8_t channel)
{	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;
	sensor->curr_channel = channel;
	
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;

	
	if (sensor->type == MCP3201)
	{		
		SPI_FullDuplex_TransmitReceive(&sensor->data[0],&sensor->data[1], 2, MCP32xx_SPI_MODE);
	}
	else if (sensor->type == MCP3202)
	{
		sensor->data[3*channel] = 0x01;
		sensor->data[1 + 3*channel] = (channel == 1) ? 0xE0 : 0xA0;
		SPI_FullDuplex_TransmitReceive(&sensor->data[3*channel],&sensor->data[3*channel], 3, MCP32xx_SPI_MODE);
	}
	else if (sensor->type == MCP3204 || sensor->type == MCP3208)
	{		
		sensor->data[3*channel] = 0x06 | ((channel & 0x04) >> 2);
		sensor->data[1 + 3*channel] = (channel & 0x03)<<6;
		SPI_FullDuplex_TransmitReceive(&sensor->data[3*channel],&sensor->data[3*channel], 3, MCP32xx_SPI_MODE);
	}
}

void MCP320x_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}


