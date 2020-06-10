/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Modified January 2019
*	by Michel Stempin <michel.stempin@wanadoo.fr>
*	Cleanup and optimizations
*
*/

#include <stm32f10x.h>
#include <stdlib.h>
#include <stdbool.h>

#include "usb.h"
#include "hid.h"

#define CNTR_MASK	(CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM)
#define ISTR_MASK	(ISTR_CTR | ISTR_RESET | ISTR_SUSP | ISTR_WKUP)

USB_RxTxBuf_t RxTxBuffer[MAX_EP_NUM];

volatile uint8_t DeviceAddress;
volatile uint16_t DeviceConfigured;
const uint16_t DeviceStatus;

void USB_PMA2Buffer(uint8_t endpoint)
{
	volatile uint32_t *btable = BTABLE_ADDR(endpoint);
	uint32_t count = RxTxBuffer[endpoint].RXL = btable[USB_COUNTn_RX] & 0x3ff;
	uint32_t *address = (uint32_t *) (PMAAddr + btable[USB_ADDRn_RX] * 2);
	uint16_t *destination = (uint16_t *) RxTxBuffer[endpoint].RXB;

	for (uint32_t i = 0; i < count; i++) 
	{
		*destination++ = *address++;
	}
}

void USB_Buffer2PMA(uint8_t endpoint)
{
	volatile uint32_t *btable = BTABLE_ADDR(endpoint);
	uint32_t count = RxTxBuffer[endpoint].TXL <= RxTxBuffer[endpoint].MaxPacketSize ?
		RxTxBuffer[endpoint].TXL : RxTxBuffer[endpoint].MaxPacketSize;
	uint16_t *address = RxTxBuffer[endpoint].TXB;
	uint32_t *destination = (uint32_t *) (PMAAddr + btable[USB_ADDRn_TX] * 2);

	/* Set transmission byte count in buffer descriptor table */
	btable[USB_COUNTn_TX] = count;
	for (uint32_t i = (count + 1) / 2; i; i--) 
	{
		*destination++ = *address++;
	}
	RxTxBuffer[endpoint].TXL -= count;
	RxTxBuffer[endpoint].TXB = address;
}

void USB_SendData(uint8_t endpoint, uint16_t *data, uint16_t length)
{
	if (endpoint > 0 && !DeviceConfigured) 
	{
		return;
	}
	RxTxBuffer[endpoint].TXL = length;
	RxTxBuffer[endpoint].TXB = data;
	USB_Buffer2PMA(endpoint);
	SET_TX_STATUS(endpoint, EP_TX_VALID);
}

void USB_Shutdown(void)
{

	/* Disable USB IRQ */
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	WRITE_REG(*ISTR, 0);
	DeviceConfigured = 0;

	/* Turn USB Macrocell off */
	WRITE_REG(*CNTR, CNTR_FRES | CNTR_PDWN);

	/* PA12: General purpose output 50 MHz open drain */
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	MODIFY_REG(GPIOA->CRH,
		GPIO_CRH_CNF12 | GPIO_CRH_MODE12,
		GPIO_CRH_CNF12_0 | GPIO_CRH_MODE12);

	/* Sinks PA12 to GND */
	WRITE_REG(GPIOA->BRR, GPIO_BRR_BR12);

	/* Disable USB Clock on APB1 */
	CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);
}

void USB_Init(void)
{

	/* Reset RX and TX lengths inside RxTxBuffer struct for all
	 * endpoints
	 */
	for (int i = 0; i < MAX_EP_NUM; i++) 
	{
		RxTxBuffer[i].RXL = RxTxBuffer[i].TXL = 0;
	}

	/* PA12: General purpose Input Float */
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	MODIFY_REG(GPIOA->CRH,
		GPIO_CRH_CNF12 | GPIO_CRH_MODE12,
		GPIO_CRH_CNF12_0);

	/* USB devices start as not configured */
	DeviceConfigured = 0;

	/* Enable USB clock */
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);

	/* Enable USB IRQ in Cortex M3 core */
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	/* CNTR_FRES = 1, CNTR_PWDN = 0 */
	WRITE_REG(*CNTR, CNTR_FRES);

	/* The following sequence is recommended:
	 * 1- FRES = 0
	 * 2- Wait until RESET flag = 1 (polling)
	 * 3- clear ISTR register
	 */

	/* CNTR_FRES = 0 */
	WRITE_REG(*CNTR, 0);

	/* Wait until RESET flag = 1 (polling) */
	while (!READ_BIT(*ISTR, ISTR_RESET)) 
	{
		;
	}

	/* Clear pending interrupts */
	WRITE_REG(*ISTR, 0);

	/* Set interrupt mask */
	WRITE_REG(*CNTR, CNTR_MASK);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	volatile uint16_t istr;

	while ((istr = READ_REG(*ISTR) & ISTR_MASK) != 0) 
	{

		/* Handle EP data */
		if (READ_BIT(istr, ISTR_CTR)) 
		{
			/* Handle data on EP */
			WRITE_REG(*ISTR, CLR_CTR);
			USB_EPHandler(READ_REG(*ISTR));
		}

		/* Handle Reset */
		if (READ_BIT(istr, ISTR_RESET)) 
		{
			WRITE_REG(*ISTR, CLR_RESET);
			USB_Reset();
		}

		/* Handle Suspend */
		if (READ_BIT(istr, ISTR_SUSP)) 
		{
			WRITE_REG(*ISTR, CLR_SUSP);

			/* If device address is assigned, then reset it */
			if (READ_REG(*DADDR) & USB_DADDR_ADD) 
			{
				WRITE_REG(*DADDR, 0);
				CLEAR_BIT(*CNTR, CNTR_SUSPM);
			}
		}

		/* Handle Wakeup */
		if (READ_BIT(istr, ISTR_WKUP)) 
		{
			WRITE_REG(*ISTR, CLR_WKUP);
		}
	}

	/* Default to clear all interrupt flags */
	WRITE_REG(*ISTR, 0);
}
