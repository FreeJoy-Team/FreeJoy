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
#include <math.h>

/**
  * @brief ADS1115 I2C Write register Function
  * @param data: Content of register
  * @param addr: Register address
	* @param size: Size of data to write
  * @retval None
  */
void ADS1115_WriteRegister(uint8_t addr , uint8_t * data, uint8_t size)
{

}

/**
  * @brief ADS1115 I2C Read register Function
  * @param data: Content of register
  * @param addr: Register address
	* @param size: Size of data to read
  * @retval None
  */
void ADS1115_ReadRegister(uint8_t addr , uint8_t * data, uint8_t size)
{
	
}

/**
  * @brief ADS1115 start burst operation command
  * @param sensor: Sensor struct
  * @retval None
  */
void ADS1115_Start(sensor_t * sensor)
{

}

/**
  * @brief ADS1115 get measured data
  * @param data: variable for storing data
  * @param sensor: Sensor struct
  * @retval status
  */
int ADS1115_GetData(uint16_t * data, sensor_t * sensor)
{
	int ret = 0;
	

	return ret;
}

/**
  * @brief ADS1115 start processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void ADS1115_StartDMA(sensor_t * sensor)
{	

}

void ADS1115_StopDMA(sensor_t * sensor)
{	

}



