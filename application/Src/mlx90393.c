/**
  ******************************************************************************
  * @file           : mlx90393.c
  * @brief          : MLX90393 sensors driver implementation
			
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

#include "mlx90393.h"
#include <math.h>

/**
  * @brief MLX90393 NOP
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90393_NOP(uint8_t mode, uint8_t * in_data)
{
	uint8_t tmp_buf[1];
	
	tmp_buf[0] = 0x00;
	
	if (mode == MLX_SPI)
	{
		SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 1, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

/**
  * @brief MLX90393 warm reset function
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90393_Reset(uint8_t mode, uint8_t * in_data)
{
	uint8_t tmp_buf[1];
	
	tmp_buf[0] = 0xF0;

	if (mode == MLX_SPI)
	{
		SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 1, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

/**
  * @brief MLX90393 SPI Write command Function
  * @param command: Command to write
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90393_WriteCommand(uint8_t mode, uint8_t command, uint8_t * in_data)
{
	uint8_t tmp_buf[2];
	
	tmp_buf[0] = command;
	tmp_buf[1] = 0;
	
	if (mode == MLX_SPI)
	{
		SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 2, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

/**
  * @brief MLX90393 SPI Write register Function
  * @param data: Content of register
  * @param addr: Register address
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90393_WriteRegister(uint8_t mode, uint16_t data,  uint8_t addr , uint8_t * in_data)
{
	uint8_t tmp_buf[5];
	
	tmp_buf[0] = 0x60;								// Write Register command	
	tmp_buf[1] = data >> 8;						// MSB
	tmp_buf[2] = data & 0xFF;					// LSB
	tmp_buf[3] = addr<<2;							// Register address	
	tmp_buf[4] = 0;										// Status
	
	if (mode == MLX_SPI)
	{
		SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 5, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

/**
  * @brief MLX90393 SPI Read register Function
  * @param data: Content of register
  * @param addr: Register address
  * @param in_data: Buffer for incoming data
  * @retval None
  */
void MLX90393_ReadRegister(uint8_t mode, uint8_t addr , uint8_t * in_data)
{
	uint8_t tmp_buf[5];
	
	tmp_buf[0] = 0x50;								// Write Register command	
	tmp_buf[1] = addr<<2;							// Register address	
	tmp_buf[2] = 0;										// Status
	tmp_buf[3] = 0;										// MSB
	tmp_buf[4] = 0;										// LSB
	
	if (mode == MLX_SPI)
	{
		SPI_FullDuplex_TransmitReceive(tmp_buf, in_data, 5, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

/**
  * @brief MLX90393 start burst operation command
  * @param sensor: Sensor struct
  * @retval None
  */
void MLX90393_Start(uint8_t mode, sensor_t * sensor)
{
	uint8_t rx_buf[5];
	uint16_t tmp_data;
	
	// Exit
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_WriteCommand(mode, MLX_EXIT, rx_buf);							
	Delay_us(50);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(20);
	
	// Reset
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_Reset(mode, rx_buf);							
	Delay_us(50);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(50);

	// -------------- Register 0 ----------------- //
	
	// Read register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_ReadRegister(mode, 0x00, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_us(10);	
	// Modify register value
	tmp_data = rx_buf[3]<<8|rx_buf[4];
	tmp_data &= ~0x01FF;
	tmp_data |= GAIN_SEL(0)|HAL_CONF(0x00);
	// Write register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_WriteRegister(mode, tmp_data, 0x00, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(20);	

	// -------------- Register 1 ----------------- //
	
	// Read register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_ReadRegister(mode, 0x01, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_us(20);
	// Set register value
	tmp_data = SPI_MODE|BURST_SEL_X|BURST_SEL_Y|BURST_SEL_Z;
	// Write register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_WriteRegister(mode, tmp_data, 0x01, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(20);

	// -------------- Register 2 ----------------- //
	
	// Read register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_ReadRegister(mode, 0x02, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_us(20);
	// Modify register value
	tmp_data = rx_buf[3]<<8|rx_buf[4];
	tmp_data &= ~0x1FFF;
	tmp_data |= RES(1,1,1)|OSR(0)|DIG_FILT(0);
	// Write register value
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_WriteRegister(mode, tmp_data, 0x02, rx_buf);					
	Delay_us(70);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(20);	
	
	// Burst mode XYZ	
	pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
	MLX90393_WriteCommand(mode, MLX_START_BURST|MLX_X|MLX_Y|MLX_Z, rx_buf);							
	Delay_us(50);
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	Delay_ms(20);
}

/**
  * @brief MLX90393 get measured data
  * @param data: variable for storing data
  * @param sensor: Sensor struct
  * @retval status
  */
int MLX90393_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel)
{
	int ret = 0;
	
	if (sensor->data[1] & 0x10)	ret = -1;
	else
	{
		*data = (sensor->data[2 + channel*2]<<8|sensor->data[3 + channel*2]);
	}
	return ret;
}

/**
  * @brief MLX90393 start processing with DMA
  * @param sensor: Sensor struct
  * @retval None
  */
void MLX90393_StartDMA(uint8_t mode, sensor_t * sensor)
{	
	uint8_t tmp_buf[8];
	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;

	tmp_buf[0] = 0x4E;		// Read Meas. command
	tmp_buf[1] = 0x00;		// Status
	tmp_buf[2] = 0x00;		// X MSB
	tmp_buf[3] = 0x00;		// X LSB
	tmp_buf[4] = 0x00;		// Y MSB
	tmp_buf[5] = 0x00;		// Y LSB
	tmp_buf[6] = 0x00;		// Z MSB
	tmp_buf[7] = 0x00;		// Z LSB
	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM2_IRQn);
	
	if (mode == MLX_SPI)
	{
		// CS low
		pin_config[sensor->source].port->ODR &= ~pin_config[sensor->source].pin;
		SPI_FullDuplex_TransmitReceive(tmp_buf, sensor->data, 8, MLX90393_SPI_MODE);
	}
	else if (mode == MLX_I2C)
	{
		
	}
}

void MLX90393_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->source].port->ODR |= pin_config[sensor->source].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}



