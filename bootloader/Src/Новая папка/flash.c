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
* Split January 2019 from hid.c
*	by Michel Stempin <michel.stempin@wanadoo.fr>
*/

#include "flash.h"

void Flash_Unlock (void)
{
	/* Unlock Flash with magic keys */
	WRITE_REG(FLASH->KEYR, FLASH_KEY1);
	WRITE_REG(FLASH->KEYR, FLASH_KEY2);
	while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
}

void Flash_Lock (void)
{
	/* Lock Flash */
	SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

void Flash_FormatPage(uint16_t * addr)
{
	/* Format page */
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
	SET_BIT(FLASH->CR, FLASH_CR_PER);
	WRITE_REG(FLASH->AR, (uint32_t) addr);
	SET_BIT(FLASH->CR, FLASH_CR_STRT);
	while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
}

void FLASH_Write(uint16_t * addr, uint16_t * data, uint16_t size)
{
	/* Write page data */
	while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
	CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	SET_BIT(FLASH->CR, FLASH_CR_PG);
	for (uint16_t i = 0; i < size; i++) 
	{
		addr[i] = data[i];
		while (READ_BIT(FLASH->SR, FLASH_SR_BSY));
	}
	CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

