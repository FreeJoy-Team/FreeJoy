/**
  ******************************************************************************
  * @file           : flash.c
  * @brief          : Flash store implementation
  ******************************************************************************
  */

#include "flash.h"


void ConfigSet (app_config_t * p_config)
{
	uint32_t data_addr = (uint32_t) p_config;
	uint32_t prog_addr;
	
	if (p_config == NULL)
		return;

	prog_addr = CONFIG_ADDR;
	
	FLASH_Unlock();
	FLASH_ErasePage(prog_addr);
	
	for (int i=0; i<sizeof(app_config_t); i+=4)
	{
		FLASH_ProgramWord(prog_addr+i, *(uint32_t *)(data_addr + i)); 

	}
	FLASH_Lock();
}

void ConfigGet (app_config_t * p_config)
{
	uint32_t read_addr;
	uint32_t data_addr = (uint32_t) p_config;
	
	if (p_config == NULL)
		return;
	
	//read_addr = FLASH_BASE + (*(uint16_t *)FLASHSIZE_BASE - 1)*1024;		// last page
	read_addr = CONFIG_ADDR;
	for (int i=0; i<sizeof(app_config_t); i+=4)
	{
		*(uint32_t *)(data_addr + i) = *(uint32_t *) (read_addr+i);
	}
	
}


