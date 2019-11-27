#include "bootloader.h"



int main(void)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitStruct;
	uint32_t PageError = 0;
	uint16_t program_size;
	
	HAL_Init();
	
	program_size = *(uint16_t*) (FIRMWARE_COPY_ADDR);
	
	FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	FLASH_EraseInitStruct.NbPages = 24;
	FLASH_EraseInitStruct.PageAddress = 0x8000000;
	
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &PageError);
	
	for (uint16_t i=0; i<program_size; i+=sizeof(uint32_t))
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x8000000 + i, *(uint32_t*) (FIRMWARE_COPY_ADDR + 0x3C + i));
	}
	HAL_FLASH_Lock();
	
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
