/**
  ******************************************************************************
  * @file           : flash.c
  * @brief          : Flash store implementation
  ******************************************************************************
  */

#include "flash.h"



void ConfigSet (app_config_t * p_config)
{
	FLASH_EraseInitTypeDef FLASH_EraseInitStruct;
	uint32_t PageError = 0;
	uint32_t data_addr = (uint32_t) p_config;
	uint32_t prog_addr;
	
	if (p_config == NULL)
		return;

	//prog_addr = FLASH_BASE + (*(uint16_t *)FLASHSIZE_BASE - 1)*1024;		// last page
	prog_addr = CONFIG_ADDR;
	
	FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	FLASH_EraseInitStruct.NbPages = 1;
	FLASH_EraseInitStruct.PageAddress = prog_addr;
	
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &PageError);
	
	for (int i=0; i<sizeof(app_config_t); i+=4)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, prog_addr+i, *(uint32_t *)(data_addr + i)); 
	}
	HAL_FLASH_Lock();
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


