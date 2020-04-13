/**
  ******************************************************************************
  * @file           : ads1115.c
  * @brief          : ADS1115 ADC driver implementation
			
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

#include "ads1115.h"

/**
  * @brief ADS1115 init function
  * @param sensor: Sensor struct
  * @retval None
  */
void ADS1115_Init(sensor_t * sensor)
{
	int status;
	uint8_t tmp_buf[2];
	
	tmp_buf[0] = 0xC3;
	tmp_buf[1] = 0xE3;
	status = I2C_WriteBlocking(sensor->address, 1, tmp_buf, 2);

	tmp_buf[0] = 0x00;
	tmp_buf[1] = 0x00;
	if (status == 0)
	{
		status = I2C_ReadBlocking(sensor->address, 1, tmp_buf, 2, 0);
	}
}

/**
  * @brief ADS1115 get measured data
  * @param sensor: Sensor struct
  * @retval data
  */
int16_t ADS1115_GetData(sensor_t * sensor, uint8_t channel)
{
	return sensor->data[2*channel]<<8|sensor->data[1 + 2*channel];
}

/**
  * @brief ADS1115 start processing data in blocking mode
  * @param sensor: Sensor struct
  * @retval status
  */
int ADS1115_ReadBlocking(sensor_t * sensor, uint8_t channel)
{
	int ret;	
	ret = I2C_ReadBlocking(sensor->address, 0, &sensor->data[2*channel], 2, 0);
	if (ret == 0 ) sensor->ok_cnt++;
	else sensor->err_cnt++;
	
	return ret;
}

/**
  * @brief ADS1115 set mux in blocking mode
  * @param sensor: Sensor struct
  * @retval status
  */
int ADS1115_SetMuxBlocking(sensor_t * sensor, uint8_t channel)
{
	int ret;
	uint8_t tmp_buf[3];
	
	tmp_buf[0] = 0xC3 | (channel << 4);							// config reg MSB
	tmp_buf[1] = 0xE3;															// config reg LSB
	
	ret = I2C_WriteBlocking(sensor->address, 1, tmp_buf, 2);
	
	sensor->curr_channel = channel;
	
	return ret;
}
