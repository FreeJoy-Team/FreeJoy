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

const char cba_256_TAB [] = { 0x00, 0x2F, 0x5E, 0x71, 0xBC, 0x93, 0xE2, 0xCD,
							  0x57, 0x78, 0x09, 0x26, 0xEB, 0xC4, 0xB5, 0x9A,
							  0xAE, 0x81, 0xF0, 0xDF, 0x12, 0x3D, 0x4C, 0x63,
							  0xF9, 0xD6, 0xA7, 0x88, 0x45, 0x6A, 0x1B, 0x34,
							  0x73, 0x5C, 0x2D, 0x02, 0xCF, 0xE0, 0x91, 0xBE,
							  0x24, 0x0B, 0x7A, 0x55, 0x98, 0xB7, 0xC6, 0xE9,
							  0xDD, 0xF2, 0x83, 0xAC, 0x61, 0x4E, 0x3F, 0x10,
							  0x8A, 0xA5, 0xD4, 0xFB, 0x36, 0x19, 0x68, 0x47,
							  0xE6, 0xC9, 0xB8, 0x97, 0x5A, 0x75, 0x04, 0x2B,
							  0xB1, 0x9E, 0xEF, 0xC0, 0x0D, 0x22, 0x53, 0x7C,
							  0x48, 0x67, 0x16, 0x39, 0xF4, 0xDB, 0xAA, 0x85,
							  0x1F, 0x30, 0x41, 0x6E, 0xA3, 0x8C, 0xFD, 0xD2,
							  0x95, 0xBA, 0xCB, 0xE4, 0x29, 0x06, 0x77, 0x58,
							  0xC2, 0xED, 0x9C, 0xB3, 0x7E, 0x51, 0x20, 0x0F,
							  0x3B, 0x14, 0x65, 0x4A, 0x87, 0xA8, 0xD9, 0xF6,
							  0x6C, 0x43, 0x32, 0x1D, 0xD0, 0xFF, 0x8E, 0xA1,
							  0xE3, 0xCC, 0xBD, 0x92, 0x5F, 0x70, 0x01, 0x2E,
							  0xB4, 0x9B, 0xEA, 0xC5, 0x08, 0x27, 0x56, 0x79,
							  0x4D, 0x62, 0x13, 0x3C, 0xF1, 0xDE, 0xAF, 0x80,
  							  0x1A, 0x35, 0x44, 0x6B, 0xA6, 0x89, 0xF8, 0xD7,
							  0x90, 0xBF, 0xCE, 0xE1, 0x2C, 0x03, 0x72, 0x5D,
							  0xC7, 0xE8, 0x99, 0xB6, 0x7B, 0x54, 0x25, 0x0A,
							  0x3E, 0x11, 0x60, 0x4F, 0x82, 0xAD, 0xDC, 0xF3,
							  0x69, 0x46, 0x37, 0x18, 0xD5, 0xFA, 0x8B, 0xA4,
							  0x05, 0x2A, 0x5B, 0x74, 0xB9, 0x96, 0xE7, 0xC8,
 							  0x52, 0x7D, 0x0C, 0x23, 0xEE, 0xC1, 0xB0, 0x9F,
							  0xAB, 0x84, 0xF5, 0xDA, 0x17, 0x38, 0x49, 0x66,
							  0xFC, 0xD3, 0xA2, 0x8D, 0x40, 0x6F, 0x1E, 0x31,
							  0x76, 0x59, 0x28, 0x07, 0xCA, 0xE5, 0x94, 0xBB,
							  0x21, 0x0E, 0x7F, 0x50, 0x9D, 0xB2, 0xC3, 0xEC,
							  0xD8, 0xF7, 0x86, 0xA9, 0x64, 0x4B, 0x3A, 0x15,
							  0x8F, 0xA0, 0xD1, 0xFE, 0x33, 0x1C, 0x6D, 0x42 };
							  
static uint8_t tmp_buf[8];

/**
  * @brief MLX90363 Checksum set/verification
  * @param message: Buffer of given message to verify
  * @retval 1 if CRC was correct, 0 if it wasn't
  */
int MLX90363_Checksum(uint8_t * message) {
	// Sets the last byte of the message to the CRC-8 of the first seven bytes
	// Also checks the existing checksum
	// Returns 1 if OK, 0 if CRC doesn't match.
	uint8_t crc = message[7];
	
	message[7] = 0xFF;
	for (uint8_t x=0; x<7; x++) {
		message[7] = cba_256_TAB[message[x] ^ message[7]];
	}
	message[7] = ~message[7];
	
	return message[7] == crc;
}

/**
  * @brief MLX90363 GET1
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90363_GET1(uint8_t * in_data) {
	tmp_buf[0] = tmp_buf[1] = tmp_buf[4] = tmp_buf[5] = NULL_DATA;
	tmp_buf[2] = tmp_buf[3] = GET_TIME_OUT;
	tmp_buf[6] = GET1_OPCODE;
	MLX90363_Checksum(tmp_buf);
	
	SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 8, MLX90363_SPI_MODE);
}

/**
  * @brief MLX90363 NOP
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90363_NOP(uint8_t * in_data) {
	tmp_buf[0] = tmp_buf[1] = tmp_buf[4] = tmp_buf[5] = NULL_DATA;
	tmp_buf[2] = tmp_buf[3] = NOP_KEY;
	tmp_buf[6] = NOP_OPCODE;
	MLX90363_Checksum(tmp_buf);
	
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
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(1);
}

/**
  * @brief MLX90363 get measured data
  * @param data: variable for storing data
  * @param sensor: Sensor struct
  * @retval status
  */
int MLX90363_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel)
{
	/*
	 * Sooo, BIT's 2, 3 should be 0x00, BIT 4 should be above 0x00, BIT 5 should be 0x00
	 */
	
	// Let's check those 2,3,5 bits if they are 0x00
	if (sensor->data[2] != 0x00 || sensor->data[3] != 0x00 || sensor->data[5] != 0x00) {
		return -1; // Probably wrong data
	}
	
	// Virtual Gain at 0?
	if (sensor->data[4] == 0x00) {
		return -1; // Probably wrong data
	}
	
	// Check the checksum of the message
	if (!MLX90363_Checksum(sensor->data)) {
		return -1; // CRC not valid
	}
	
	// Check the marker if we've got Alpha (0), otherwise ignore
	if ((sensor->data[6] >> 6) != 0) {
		return -1; // Not Alpha result
	}
	
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
	
	// ToDo: Add a check if last MLX90363_StartDMA was called more than maximum time out rate of 65ms.
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
