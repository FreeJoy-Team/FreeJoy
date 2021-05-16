/**
  ******************************************************************************
  * @file           : mlx90363.c
  * @brief          : MLX90363 sensors driver implementation
			
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

#include "mlx90363.h"

/**
  * @brief MLX90363 GET1
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90363_GET1(uint8_t * in_data) {
	uint8_t tmp_buf[8] = { 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x13, 0xEA };
	SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 8, MLX90363_SPI_MODE);
}

/**
  * @brief MLX90363 NOP
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90363_NOP(uint8_t * in_data)
{
	uint8_t tmp_buf[8] = { 0x00, 0x00, 0xAA, 0xAA, 0x00, 0x00, 0xD0, 0xAB };
	SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 8, MLX90363_SPI_MODE);
}

/**
  * @brief MLX90363 start command - First GET1 won't return the data. That's why we need to sent it on 1st time.
  * @param sensor: Sensor struct
  * @retval None
  */
void MLX90363_Start(sensor_t * sensor)
{
	uint8_t rx_buf[8];

	// First GET1
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90363_GET1(rx_buf);
	Delay_us(50);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(5);
}

/**
  * @brief MLX90363 get measured data
  * @param data: variable for storing data
  * @param sensor: Sensor struct
  * @retval status
  */
int MLX90363_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel)
{
	// Remove error bits, shift to high byte and add LSB of angle
	*data = ((sensor->data[1] & 0x3F) << 8) + sensor->data[0];

	return 0; // No errors
}

/**
  * @brief MLX90363 start processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void MLX90363_StartDMA(sensor_t * sensor)
{	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;
	
	// ToDo: Add check if last MLX90363_StartDMA was called more than maximum time out rate of 65ms.
	// When this happens it's necessary to interlace GET1 and NOP messages to avoid receiving the NTT error.
	// I don't know if it may happen but if it can then we need a timer so if that happens, we call
	// MLX90363_NOP first, set the CS to high and then again to low to send MLX90363_GET1.
	
	// CS low
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90363_GET1(sensor->data);
}

/**
  * @brief MLX90363 stops processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void MLX90363_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}
