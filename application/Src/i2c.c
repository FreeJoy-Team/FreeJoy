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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	

	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;	
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
	
	I2C_ITConfig(I2C1,I2C_IT_ERR,ENABLE);	
	NVIC_EnableIRQ (I2C1_ER_IRQn);
	NVIC_SetPriority(I2C1_ER_IRQn,3);
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
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	ticks = I2C_TIMEOUT;
	if (ticks == 0) return -1;
	
	I2C_Send7bitAddress(I2C1, dev_addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_SendData(I2C1, reg_addr);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
		
	for (uint16_t i=0; i<length; i++)
	{
		I2C_SendData(I2C1, data[i]);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && --ticks);
		if (ticks == 0) return -1;
		ticks = I2C_TIMEOUT;
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && --ticks);
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
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
		
	I2C_Send7bitAddress(I2C1, dev_addr<<1, I2C_Direction_Transmitter);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;

	I2C_SendData(I2C1, reg_addr);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C1, dev_addr<<1, I2C_Direction_Receiver);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) return -1;
	ticks = I2C_TIMEOUT;
	
	for (uint8_t i=0; i<length; i++)
	{		
		while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && --ticks);
		if (ticks == 0) return -1;
		ticks = I2C_TIMEOUT;
		
		data[i] = I2C1->DR;
		
		if (nack && i == length - 2) 
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
		}
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) return -1;
	
	return 0;
}


