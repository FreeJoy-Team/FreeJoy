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
void I2C_Start(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	

	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x07;	
	I2C_Init(I2C2,&I2C_InitStructure);
	I2C_Cmd(I2C2,ENABLE);
	
	I2C_ITConfig(I2C2,I2C_IT_ERR,ENABLE);	
	NVIC_EnableIRQ (I2C2_ER_IRQn);
	NVIC_SetPriority(I2C2_ER_IRQn,2);
}

/**
  * @brief Hardware I2C Send Function
	* @param dev_addr: slave device 7-bit I2C address
	* @param reg_addr: address of internal register
	* @param data: data to transmit
	* @param length: length of data to transmit
  * @retval status
  */
int I2C_WriteBlocking(uint8_t dev_addr, uint8_t reg_addr, uint8_t * data, uint16_t length)
{	
	uint32_t ticks = I2C_TIMEOUT;			// number of flag checks before stop i2c transmition 
	
	I2C_DMACmd(I2C2, DISABLE);
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C2, dev_addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_SendData(I2C2, reg_addr);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
		
	for (uint16_t i=0; i<length; i++)
	{
		I2C_SendData(I2C2, data[i]);
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && --ticks);
		if (ticks == 0) return -1;
		ticks = I2C_TIMEOUT;
	}
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	
	return 0;
}

/**
  * @brief Hardware I2C Receive Function
	* @param dev_addr: slave device 7-bit I2C address
	* @param reg_addr: address of internal register
	* @param data: buffer for storing received data
	* @param length: length of data to receive
	* @param stop: generate STOP signal before read cycle
	* @param stop: do not generate ACK after last read byte
  * @retval None
  */
int I2C_ReadBlocking(uint8_t dev_addr, uint8_t reg_addr, uint8_t * data, uint16_t length, uint8_t nack)
{
	uint32_t ticks = I2C_TIMEOUT;
	
	I2C_DMACmd(I2C2, DISABLE);
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
		
	I2C_Send7bitAddress(I2C2, dev_addr<<1, I2C_Direction_Transmitter);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;

	I2C_SendData(I2C2, reg_addr);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C2, dev_addr<<1, I2C_Direction_Receiver);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	for (uint8_t i=0; i<length; i++)
	{		
		while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) && --ticks);
		if (ticks == 0) return -1;
		ticks = I2C_TIMEOUT;
		
		data[i] = I2C2->DR;
		
		if (nack && i == length - 2) 
		{
			I2C_AcknowledgeConfig(I2C2, DISABLE);
			I2C_NACKPositionConfig(I2C2, I2C_NACKPosition_Current);
		}
	}
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	
	return 0;
}

/**
  * @brief Hardware I2C Send Function
	* @param dev_addr: slave device 7-bit I2C address
	* @param data: data to transmit
	* @param length: length of data to transmit
  * @retval status
  */
int I2C_WriteNonBlocking(uint8_t dev_addr, uint8_t * data, uint16_t length)
{	
	uint32_t ticks = I2C_TIMEOUT;			// number of flag checks before stop i2c transmition 
	
	DMA_InitTypeDef DMA_InitStructure;
		
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C2->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	
	// wait a bit if I2C is busy
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) 
	{
		I2C2->CR1 |= I2C_CR1_SWRST;
		I2C2->CR1 &= ~I2C_CR1_SWRST;
		I2C_Start();
		//return -1;
	}
	ticks = I2C_TIMEOUT;
	
	// Start DMA
	DMA_Cmd(DMA1_Channel4, ENABLE);
  I2C_DMACmd(I2C2, ENABLE);
	
	// Generate start signal
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C2, dev_addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	return 0;
}

/**
  * @brief Hardware I2C Receive Function
	* @param dev_addr: slave device 7-bit I2C address
	* @param reg_addr: address of internal register
	* @param data: buffer for storing received data
	* @param length: length of data to receive
	* @param stop: generate STOP signal before read cycle
	* @param stop: do not generate ACK after last read byte
  * @retval None
  */
int I2C_ReadNonBlocking(uint8_t dev_addr, uint8_t reg_addr, uint8_t * data, uint16_t length, uint8_t nack)
{
	uint32_t ticks = I2C_TIMEOUT;			// number of flag checks before stop i2c transmition 

	// disable DMA
	I2C_DMACmd(I2C2, DISABLE);
	DMA_Cmd(DMA1_Channel5, DISABLE);
	while (DMA1_Channel5->CCR & (1<<0));
	// clear flags
	DMA_ClearFlag(DMA1_FLAG_GL5|DMA1_FLAG_HT5|DMA1_FLAG_TC5|DMA1_FLAG_TE5);	
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C2->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel5_IRQn, 2);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	
	// wait a bit if I2C is busy
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) 
	{
		I2C2->CR1 |= I2C_CR1_SWRST;
		I2C2->CR1 &= ~I2C_CR1_SWRST;
		I2C_Start();
		//return -1;
	}
	ticks = I2C_TIMEOUT;

  // Enable DMA NACK automatic generation
	if (nack)	I2C_DMALastTransferCmd(I2C2, ENABLE);
 
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);			// EV5
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C2, dev_addr << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);			// EV6
	if (ticks == 0) return -1;
	
	// Clear EV6 by setting again the PE bit
  I2C_Cmd(I2C2, ENABLE);
	
	I2C_SendData(I2C2, reg_addr);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTOP(I2C2, ENABLE);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	
	// Start reading conversion register in non-blocking mode	
	I2C_DMACmd(I2C2,ENABLE);
	DMA_Cmd(DMA1_Channel5,ENABLE);
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C2, dev_addr << 1, I2C_Direction_Receiver);
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	return 0;
}







