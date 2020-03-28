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

void MLX90393_ReadRegister(uint8_t * data, uint8_t addr)
{
	uint8_t tmp_buf[5];
	
	tmp_buf[0] = 0x50;				// Read Register command
	tmp_buf[1] = addr<<2;			// Register address	
	tmp_buf[2] = 0;						// Status
	tmp_buf[3] = 0;						// MSB
	tmp_buf[4] = 0;						// LSB
	
	HardSPI_FullDuplex_TransmitReceive(tmp_buf, tmp_buf, 5);
}

void MLX90393_WriteRegister(uint8_t * data, uint8_t addr)
{
	uint8_t tmp_buf[5];
	
	tmp_buf[0] = 0x60;				// Write Register command	
	tmp_buf[1] = 0;						// MSB
	tmp_buf[2] = 0;						// LSB
	tmp_buf[3] = addr<<2;			// Register address	
	tmp_buf[4] = 0;						// Status
	
	HardSPI_FullDuplex_TransmitReceive(tmp_buf, tmp_buf, 5);
}

void MLX90393_StartBurst(sensor_t * sensor)
{
	uint8_t tmp_buf[2];
	
	tmp_buf[0] = 0xE8;				// MSB	
	tmp_buf[1] = 0x07;				// LSB
	
	pin_config[sensor->cs_pin].port->ODR &= ~pin_config[sensor->cs_pin].pin;
	MLX90393_WriteRegister(tmp_buf, 0x02);
	Delay_ms(1);
	pin_config[sensor->cs_pin].port->ODR |= pin_config[sensor->cs_pin].pin;
	Delay_ms(15);
	
	tmp_buf[0] = 0x1E;				// Burst mode	
	tmp_buf[1] = 0;						// Status
	
	pin_config[sensor->cs_pin].port->ODR &= ~pin_config[sensor->cs_pin].pin;
	HardSPI_FullDuplex_TransmitReceive(tmp_buf, tmp_buf, 2);
	Delay_ms(1);
	pin_config[sensor->cs_pin].port->ODR |= pin_config[sensor->cs_pin].pin;
	Delay_ms(15);
}

void MLX90393_StartDMA(sensor_t * sensor)
{	
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;
	
	// CS low
	pin_config[sensor->cs_pin].port->ODR &= ~pin_config[sensor->cs_pin].pin;
	
	sensor->data[0] = 0x4E;		// Read Meas. command
	sensor->data[1] = 0x00;		// Status
	sensor->data[2] = 0x00;		// X MSB
	sensor->data[3] = 0x00;		// X LSB
	sensor->data[4] = 0x00;		// Y MSB
	sensor->data[5] = 0x00;		// Y LSB
	sensor->data[6] = 0x00;		// Z MSB
	sensor->data[7] = 0x00;		// Z LSB
	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	
	HardSPI_FullDuplex_TransmitReceive(&sensor->data[0],&sensor->data[0], 3);
}

void MLX90393_StopDMA(sensor_t * sensor)
{	
	DMA_Cmd(DMA1_Channel2, DISABLE);
	// CS high
	pin_config[sensor->cs_pin].port->ODR |= pin_config[sensor->cs_pin].pin;
	sensor->rx_complete = 1;
	sensor->tx_complete = 1;
}



