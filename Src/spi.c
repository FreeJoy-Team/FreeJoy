/**
  ******************************************************************************
  * @file           : spi.c
  * @brief          : SPI driver implementation
			
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

#include "spi.h"

uint8_t spi_inbuf[10];
uint8_t spi_outbuf[10];

/**
  * @brief Software SPI Initialization Function
  * @param None
  * @retval None
  */
void SoftSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef	GPIO_InitStructureure;	
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
	GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (GPIOB,&GPIO_InitStructureure);
}
/**
  * @brief Software SPI Send Half-Duplex Function
  * @param None
  * @retval None
  */
void SoftSPI_HalfDuplex_Transmit(uint8_t * data, uint16_t length)
{
	GPIO_InitTypeDef 					GPIO_InitStructureure;
	
	GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructureure);
	// Set SCK low
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
	
	for (uint16_t i=0; i<length; i++)
	{
		int8_t j = 7;
		do
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (data[i] & (1<<j)) ? Bit_SET : Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			__NOP();
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			
			j--;
		} while(j>=0);
	}
}
/**
  * @brief Software SPI Receive Half-Duplex Function
  * @param None
  * @retval None
  */
void SoftSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length)
{
	GPIO_InitTypeDef 					GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Set SCK low
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
	
	for (uint16_t i=0; i<length; i++)
	{
		data[i] = 0;
		int8_t j = 7;
		do
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);			
			data[i] |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) << j;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			
			j--;
		} while(j>=0);
	}
}

/**
  * @brief Software SPI Initialization Function
  * @param None
  * @retval None
  */
void HardSPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	
	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	// SPI1 configuration
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

#if SPI_USE_DMA	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx|SPI_I2S_DMAReq_Rx, ENABLE);
#endif	
  SPI_Cmd(SPI1, ENABLE);
}
/**
  * @brief Hardware SPI Send Half-Duplex Function
  * @param None
  * @retval None
  */
void HardSPI_HalfDuplex_Transmit(uint8_t * data, uint16_t length)
{	
	GPIO_InitTypeDef 					GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init (GPIOB,&GPIO_InitStructure);

#if SPI_USE_DMA	
	DMA_InitTypeDef DMA_InitStructure;
		
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel3_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Channel3_IRQn);
	
	SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);
	
	DMA_Cmd(DMA1_Channel3, ENABLE);
	
#else
	for (uint16_t i=0; i<length; i++)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //wait buffer empty
    SPI_I2S_SendData(SPI1, data[i]);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET); //wait finish sending
	}
#endif
}
/**
  * @brief Hardware SPI Receive Half-Duplex Function
  * @param None
  * @retval None
  */
void HardSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length)
{
#if SPI_USE_DMA	
	DMA_InitTypeDef DMA_InitStructure;
		
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) data;
	DMA_InitStructure.DMA_BufferSize = length;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	NVIC_SetPriority(DMA1_Channel2_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	
//	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
//	NVIC_EnableIRQ(SPI1_IRQn);
	
	SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);
	
	DMA_Cmd(DMA1_Channel2, ENABLE);
	
#else
	for (uint16_t i=0; i<length; i++)
	{
		SPI_I2S_ReceiveData(SPI1);
    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);
    while (!(SPI1->SR & SPI_I2S_FLAG_RXNE)) ; // wait data received
    SPI1->CR1 |= SPI_Direction_Tx;  // Set Tx mode to stop Rx clock
    data[i] = SPI_I2S_ReceiveData(SPI1);
	}
#endif
}

void UserSPI_Init(void)
{
#if USE_SOFT_SPI
	SoftSPI_Init();
#else
	HardSPI_Init();
#endif
}
void UserSPI_HalfDuplex_Transmit(uint8_t * data, uint16_t length)
{
#if USE_SOFT_SPI
	SoftSPI_HalfDuplex_Transmit(data, length);
#else
	HardSPI_HalfDuplex_Transmit(data, length);
#endif	
}
void UserSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length)
{
#if USE_SOFT_SPI
	SoftSPI_HalfDuplex_Receive(data, length);
#else
	HardSPI_HalfDuplex_Receive(data, length);
#endif	
}


