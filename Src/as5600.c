/**
  ******************************************************************************
  * @file           : as5600.c
  * @brief          : AS5600 magnetic encoder driver implementation
			
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

#include "as5600.h"

/**
  * @brief AS5600 init fuction
  * @param sensor: Sensor struct
  * @retval None
  */
void AS5600_Init(sensor_t * sensor)
{
//	int status;
//	uint8_t tmp_buf[2];

//	tmp_buf[0] = 0x00;
//	tmp_buf[1] = 0x18;
//	
//	status = I2C_WriteBlocking(sensor->address, 0x07, tmp_buf, 2);
}


/**
  * @brief AS5600 get measured data
  * @param sensor: Sensor struct
  * @retval data
  */
int16_t AS5600_GetData(sensor_t * sensor)
{
	return sensor->data[0]<<8|sensor->data[1];
}

/**
  * @brief AS5600 start processing data in blocking mode
  * @param sensor: Sensor struct
  * @retval status
  */
int AS5600_ReadBlocking(sensor_t * sensor)
{
	int ret;	
	ret = I2C_ReadBlocking(sensor->address, 0x0C, &sensor->data[0], 2, 1);
	if (ret == 0 ) sensor->ok_cnt++;
	else sensor->err_cnt++;
	
	return ret;
}

