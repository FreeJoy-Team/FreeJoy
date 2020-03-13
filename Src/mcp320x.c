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

void MCP320x_Read(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = 0x80 | (addr & 0x0F)<<3 | (length & 0x07);
	
	HardSPI_HalfDuplex_Transmit(&cmd, 1);
	if (length > 0)
	{
		HardSPI_HalfDuplex_Receive(data, length+1);
	}

}

void MCP320x_Write(uint8_t * data, uint8_t addr, uint8_t length)
{
	uint8_t cmd = addr<<3 | (addr & 0x0F)<<3 | (length & 0x07);
	HardSPI_HalfDuplex_Transmit(&cmd, 1);
	if (length > 0)
	{
		HardSPI_HalfDuplex_Transmit(data, length);
	}
}

uint16_t MCP320x_GetData(sensor_t * sensor)
{
	uint16_t ret = (sensor->data[1] & 0x0F)<<8 | sensor->data[2];
	
	return ret;
}

void MCP320x_StartDMA(sensor_t * sensor)
{	
	
	// CS low
	pin_config[sensor->cs_pin].port->ODR &= ~pin_config[sensor->cs_pin].pin;

	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	
	if (sensor->type == MCP3201)
	{
		sensor->rx_complete = 0;
		sensor->tx_complete = 1;
		HardSPI_FullDuplex_TransmitReceive(&sensor->data[0],&sensor->data[1], 2);
	}
	else if (sensor->type == MCP3202)
	{
		sensor->rx_complete = 0;
		sensor->tx_complete = 1;
		sensor->data[0] = 0x01;
		sensor->data[1] = (sensor->channel == 1) ? 0xE0 : 0xA0;
		HardSPI_FullDuplex_TransmitReceive(&sensor->data[0],&sensor->data[0], 3);
	}
	else if (sensor->type == MCP3204 || sensor->type == MCP3208)
	{
		sensor->rx_complete = 0;
		sensor->tx_complete = 1;		
		sensor->data[0] = 0x06 | ((sensor->channel & 0x04) >> 2);
		sensor->data[1] = (sensor->channel & 0x03)<<6;
		HardSPI_FullDuplex_TransmitReceive(&sensor->data[0],&sensor->data[0], 3);
	}
}

void MCP320x_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);

	// CS high
	pin_config[sensor->cs_pin].port->ODR |= pin_config[sensor->cs_pin].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}


