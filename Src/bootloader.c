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
