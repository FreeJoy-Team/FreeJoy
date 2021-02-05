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

uint8_t gtmp_buf[2];

void reset_err_flag(sensor_t * sensor){
	uint16_t tmp;
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	gtmp_buf[0] = 0x40;
	gtmp_buf[1] = 0x01;
	
	SPI_HalfDuplex_Transmit(&gtmp_buf[0], 2, AS5048A_SPI_MODE);
	
	do{
		tmp = DMA_GetCurrDataCounter(DMA1_Channel3);
	}  while(tmp!=0);
	
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	
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
	// wait till the DMA channel has finished
	do{
		tmp = DMA_GetCurrDataCounter(DMA1_Channel2);
	}  while(tmp!=0);
	tmp = sensor->data[0];
	tmp = (tmp << 8) | sensor->data[1];
	*data = tmp & 0x3FFF;
	// check error bit
	if((tmp&0x4000)!=0){
		// reset err flag with 0x4001 command
		reset_err_flag(sensor);
		ret = -1;
	}
	// check parity
	tmp ^= tmp >> 8;
	tmp ^= tmp >> 4;
	tmp ^= tmp >> 2;
	tmp ^= tmp >> 1;
	if((tmp & 1)==1) ret = -2;
	return ret;
}


/**
  * @brief AS5048A start processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void AS5048A_StartDMA(sensor_t * sensor)
{	
	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;

	gtmp_buf[0] = 0x3F;		// Read Meas. command: 0x3FFF
	gtmp_buf[1] = 0xFF;		// 
	
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	SPI_FullDuplex_TransmitReceive(gtmp_buf, sensor->data, 2, AS5048A_SPI_MODE);
}

void AS5048A_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;

}



