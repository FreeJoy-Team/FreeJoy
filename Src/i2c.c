/**
  ******************************************************************************
  * @file           : i2c.c
  * @brief          : I2C driver implementation
			
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

#include "i2c.h"

/**
  * @brief Software I2C Initialization Function
  * @param None
  * @retval None
  */
void HardI2C_Init(void)
{
	I2C_InitTypeDef I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x07;
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_DMACmd(I2C1, ENABLE);
	I2C_Cmd(I2C1, ENABLE);
}

/**
  * @brief Hardware I2C Send Function
	* @param data: data to transmit
	* @param length: length of data to transmit
  * @retval None
  */
void HardI2C_Transmit(uint8_t * data, uint16_t length)
{	
	DMA_InitTypeDef DMA_InitStructure;
		
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel6_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

/**
  * @brief Hardware I2C Receive Function
	* @param data: buffer for storing received data
	* @param length: length of data to receive
  * @retval None
  */
void HardI2C_Receive(uint8_t * data, uint16_t length)
{
	DMA_InitTypeDef DMA_InitStructure;
		
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel7_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
	DMA_Cmd(DMA1_Channel7, ENABLE);
}



