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
  * @brief ADS1115 start burst operation command
  * @param sensor: Sensor struct
  * @retval None
  */
void ADS1115_Init(sensor_t * sensor)
{
	int status;
	uint8_t tmp_buf[2];
	
	GPIOB->ODR |= GPIO_Pin_12;
	tmp_buf[0] = 0xC3;
	tmp_buf[1] = 0xE3;
	status = I2C_WriteBlocking(sensor->address << 1, 1, tmp_buf, 2);

	tmp_buf[0] = 0x00;
	tmp_buf[1] = 0x00;
	if (status == 0)
	{
		status = I2C_ReadBlocking(sensor->address << 1, 1, tmp_buf, 2);
	}
	GPIOB->ODR &= ~GPIO_Pin_12;
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
	ret = I2C_ReadBlocking(sensor->address << 1, 0, &sensor->data[2*channel], 2);

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
	
	ret = I2C_WriteBlocking(sensor->address << 1, 1, tmp_buf, 2);
	
	sensor->curr_channel = channel;
	
	return ret;
}

#ifdef ADS1115_DMA_MODE

/**
  * @brief ADS1115 start processing data with DMA
  * @param sensor: Sensor struct
  * @retval status
  */
int ADS1115_StartDMA(sensor_t * sensor, uint8_t channel)
{	
	uint32_t ticks = I2C_TIMEOUT;
	
	// disable DMA
	I2C_DMACmd(I2C1, DISABLE);
	DMA_Cmd(DMA1_Channel7, DISABLE);
	while (DMA1_Channel7->CCR & (1<<0));
	// clear flags
	DMA_ClearFlag(DMA1_FLAG_GL7|DMA1_FLAG_HT7|DMA1_FLAG_TC7|DMA1_FLAG_TE7);	
	
	// DMA initialization
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &sensor->data[2*channel];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_BufferSize = 2;	
	DMA_Init(DMA1_Channel7,&DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	// set transmittion flags
	sensor->rx_complete = 0;
	sensor->tx_complete = 1;
	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	
	// Writting pointer register in blocking mode 
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);			// EV5
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C1, sensor->address << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);			// EV6
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	
	I2C_SendData(I2C1, 0x00);		// Pointer register data is 0x00 (address of conversion register)
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && --ticks);
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	ticks = I2C_TIMEOUT;
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && --ticks);
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	
	// Start reading conversion register in non-blocking mode	
	I2C_DMALastTransferCmd(I2C1, ENABLE);
	I2C_DMACmd(I2C1,ENABLE);
	DMA_Cmd(DMA1_Channel7,ENABLE);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && --ticks);
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	ticks = I2C_TIMEOUT;
	
	I2C_Send7bitAddress(I2C1, sensor->address << 1, I2C_Direction_Receiver);
	while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && --ticks);
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	ticks = I2C_TIMEOUT;
	
	return 0;
}

/**
  * @brief ADS1115 set mux with DMA
  * @param sensor: Sensor struct
  * @retval status
  */
int ADS1115_SetMuxDMA(sensor_t * sensor, uint8_t channel)
{
	uint8_t tmp_buf[3];
	uint32_t ticks = I2C_TIMEOUT;
	
	tmp_buf[0] = 0x01;															// config register address
	tmp_buf[1] = 0xC3 | (channel << 4);							// config reg MSB
	tmp_buf[2] = 0xE3;															// config reg LSB
	
	sensor->rx_complete = 1;
	sensor->tx_complete = 0;
	sensor->curr_channel = channel;
	
	// disable DMA
	I2C_DMACmd(I2C1,DISABLE);
	DMA_Cmd(DMA1_Channel6,DISABLE);
	while (DMA1_Channel6->CCR & (1<<0));
	// clear flags
	DMA_ClearFlag(DMA1_FLAG_GL6|DMA1_FLAG_HT6|DMA1_FLAG_TC6|DMA1_FLAG_TE6);	
	
	// DMA initialization
	DMA_InitTypeDef DMA_InitStructure;
	//DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) tmp_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_BufferSize = sizeof(tmp_buf);	
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	
	// Disable other interrupts
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	
	// Start reading conversion register in non-blocking mode
	I2C_DMACmd(I2C1,ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
	// Start writtin config register in non-blocking mode
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && --ticks);
	ticks = I2C_TIMEOUT;
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	
	I2C_Send7bitAddress(I2C1, sensor->address << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && --ticks);
	if (ticks == 0) 
	{
		sensor->rx_complete = 1;
		sensor->tx_complete = 1;
		return -1;
	}
	ticks = I2C_TIMEOUT;
	
	return 0;
}

#endif	/* ADS1115_DMA_MODE */

