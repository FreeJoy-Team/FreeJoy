/**
  ******************************************************************************
  * @file           : bootloader.c
  * @brief          : Application bootloader
	
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

#include "bootloader.h"


int main(void)
{

	uint16_t program_size;
	
	program_size = *(uint16_t*) (FIRMWARE_COPY_ADDR);
	
	// erase program
	FLASH_Unlock();
	for (uint8_t i=0; i<28; i++)
	{
		FLASH_ErasePage(0x8000000 + i*0x400);
	}
	
	// erase config
	FLASH_ErasePage(CONFIG_ADDR);
	
	for (uint16_t i=0; i<program_size; i+=sizeof(uint32_t))
	{
		FLASH_ProgramWord(0x8000000 + i, *(uint32_t*) (FIRMWARE_COPY_ADDR + 0x3C + i));
	}
	FLASH_Lock();
	
	EnterProgram();
}

void EnterProgram (void)
{
	uint32_t program_addr = *(uint32_t*) 0x8000004;
	typedef void(*pFunction)(void);
	pFunction Program;
	
	__disable_irq();
	
	Program = (pFunction) program_addr;
	
	__set_MSP(*(__IO uint32_t*) (program_addr - 4));
	
	Program();
}
