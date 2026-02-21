/**
  ******************************************************************************
  * @file           : uart.c
  * @brief          : UART implementation
			
		FreeJoy software for game device controllers
    Copyright (C) 2020  Yury Vostrenkov (yuvostrenkov@gmail.com)
		and Reksotiv (https://github.com/Reksotiv)

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

#include "uart.h"
#include "stm32f10x_usart.h"


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void UART_Start(void)
{
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* Enable UART Clock */
  RCC_APB2PeriphClockCmd(UART_CLK, ENABLE);
	
	
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	/* Configure UART */
	USART_Init(UART, &USART_InitStructure);
	
	/* Enable UART DMA TX request */
  //USART_DMACmd(UART, USART_DMAReq_Tx, ENABLE);
	
	/* Enable UART TX DMA1 Channel */
  //DMA_Cmd(UART_Tx_DMA_Channel, ENABLE);
	
	/* Enable the UART */
  USART_Cmd(UART, ENABLE);
}


/**
  * @brief Hardware UART Send Function
	* @param data: data to transmit
	* @param length: length of data to transmit
  */
void UART_WriteNonBlocking(uint8_t * data, uint16_t length)
{
	DMA_InitTypeDef DMA_InitStructure;
		
	/* UART TX DMA1 Channel (triggered by UART Tx event) Config */
  DMA_DeInit(UART_Tx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = UART_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)data;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = length;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_Medium
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(UART_Tx_DMA_Channel, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel4_IRQn, 2);//2
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	
	// Start DMA
	/* Enable UART TX DMA1 Channel */
  DMA_Cmd(UART_Tx_DMA_Channel, ENABLE);
	/* Enable UART DMA TX request */
  USART_DMACmd(UART, USART_DMAReq_Tx, ENABLE);
}


/**
  * @brief CRC16 calc with polynominal 0x8005
	* @param data: data for calc
	* @param size: size of data
  */
uint16_t gen_crc16(const uint8_t *data, uint16_t size)
{
	uint16_t out = 0, crc = 0;
	int32_t bits_read = 0, bit_flag = 0, i = 0;
	int32_t j = 0x0001;
	
	if (data == NULL) return 0;
	
	while (size > 0)
	{
		bit_flag = out >> 15;
		out <<= 1;
		out |= (*data >> bits_read) & 1;
		
		bits_read++;
		if (bits_read > 7)
		{
			bits_read = 0;
			data++;
			size --;
		}
		
		if (bit_flag) out ^= CRC16;
	}
	
	for (i = 0; i < 16; ++i)
	{
		bit_flag = out >> 15;
		out <<= 1;
		if (bit_flag) out ^= CRC16;
	}
	
	i = 0x8000;
	for (; i !=0; i >>= 1, j <<= 1)
	{
		if (i & out) crc |= j;
	}
	
	return crc;
}
