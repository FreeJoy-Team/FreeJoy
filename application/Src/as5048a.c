/**
  ******************************************************************************
  * @file           : as5048a.c
  * @brief          : AS5048A sensors driver implementation
			
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

#include "as5048a.h"
#include <math.h>

/**
  * @brief AS5048A start operation command
  * @param sensor: Sensor struct
  * @retval None
  */
void AS5048A_Start(sensor_t * sensor)
{
}

/**
  * @brief AS5048A get measured data
  * @param data: variable for storing data
  * @param sensor: Sensor struct
  * @retval status
  */
int AS5048A_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel)
{
	int ret = 0;
	uint16_t tmp;
	tmp = sensor->data[0];
	tmp = tmp & 0x3F;
	tmp = tmp << 8;
	*data = tmp | sensor->data[1];
	return ret;
}

/**
  * @brief AS5048A start processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void AS5048A_StartDMA(sensor_t * sensor)
{	
uint8_t tmp_buf[8];
	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;

	tmp_buf[0] = 0x3F;		// Read Meas. command: 0x3FFF
	tmp_buf[1] = 0xFF;		// 
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	SPI_FullDuplex_TransmitReceive(tmp_buf, sensor->data, 2, AS5048A_SPI_MODE);
}

void AS5048A_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}



