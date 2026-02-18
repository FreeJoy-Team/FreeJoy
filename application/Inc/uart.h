/**
  ******************************************************************************
  * @file           : uart.h
  * @brief          : Header for uart.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H__
#define __UART_H__

#include <stddef.h>
#include <stdint.h>

#define UART                   USART1
#define UART_GPIO              GPIOA
#define UART_CLK               RCC_APB2Periph_USART1
#define UART_GPIO_CLK          RCC_APB2Periph_GPIOA
#define UART_RxPin             GPIO_Pin_10
#define UART_TxPin             GPIO_Pin_9
#define UART_Tx_DMA_Channel    DMA1_Channel4
#define UART_Tx_DMA_FLAG       DMA1_FLAG_TC4
#define UART_Rx_DMA_Channel    DMA1_Channel5
#define UART_Rx_DMA_FLAG       DMA1_FLAG_TC5  
#define UART_DR_Base           0x40013804

#define CRC16									 0x8005

void UART_Start(void);
void UART_WriteNonBlocking(uint8_t * data, uint16_t length);

uint16_t gen_crc16(const uint8_t *data, uint16_t size);

#endif 	/* __UART_H__ */
