#include "spi.h"


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
  SPI_Cmd(SPI1, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
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
	
	for (uint16_t i=0; i<length; i++)
	{
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); //wait buffer empty
    SPI_I2S_SendData(SPI1, data[i]);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET); //wait finish sending
	}
}
/**
  * @brief Hardware SPI Receive Half-Duplex Function
  * @param None
  * @retval None
  */
void HardSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length)
{
	for (uint16_t i=0; i<length; i++)
	{
		SPI_I2S_ReceiveData(SPI1);
    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);
    while (!(SPI1->SR & SPI_I2S_FLAG_RXNE)) ; // wait data received
    SPI1->CR1 |= SPI_Direction_Tx;  // Set Tx mode to stop Rx clock
    data[i] = SPI_I2S_ReceiveData(SPI1);
	}
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


