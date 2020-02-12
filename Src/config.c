/**
  ******************************************************************************
  * @file           : config.c
  * @brief          : Config management implementation
  ******************************************************************************
  */

#include "config.h"

app_config_t app_config;

void DevConfigSet (dev_config_t * p_dev_config)
{
	uint32_t data_addr = (uint32_t) p_dev_config;
	uint32_t prog_addr;
	
	if (p_dev_config == NULL)
		return;

	prog_addr = CONFIG_ADDR;
	
	FLASH_Unlock();
	FLASH_ErasePage(prog_addr);
	
	for (int i=0; i<sizeof(dev_config_t); i+=4)
	{
		FLASH_ProgramWord(prog_addr+i, *(uint32_t *)(data_addr + i)); 

	}
	FLASH_Lock();
}

void DevConfigGet (dev_config_t * p_dev_config)
{
	uint32_t read_addr;
	uint32_t data_addr = (uint32_t) p_dev_config;
	
	if (p_dev_config == NULL)
		return;
	
	//read_addr = FLASH_BASE + (*(uint16_t *)FLASHSIZE_BASE - 1)*1024;		// last page
	read_addr = CONFIG_ADDR;
	for (int i=0; i<sizeof(dev_config_t); i+=4)
	{
		*(uint32_t *)(data_addr + i) = *(uint32_t *) (read_addr+i);
	}
	
}

void AppConfigInit (dev_config_t * p_dev_config)
{
	app_config.axes_cnt = 0;
	app_config.buttons_cnt = 0;
	app_config.povs = 0;
	
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		if (p_dev_config->axis_config[i].out_enabled)	app_config.axes_cnt++;
	}

	for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
	{
		if (p_dev_config->buttons[i].physical_num >= 0)	app_config.buttons_cnt++;
	}		
	
	for (uint8_t i=0; i<4; i++)
	{
		for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
		{
			if (p_dev_config->buttons[j].physical_num >= 0 &&  
				 (p_dev_config->buttons[j].type == POV1_DOWN || p_dev_config->buttons[j].type == POV1_UP ||
					p_dev_config->buttons[j].type == POV1_LEFT || p_dev_config->buttons[j].type == POV1_RIGHT))
			{
				app_config.povs |= (1<<i);
				break;
			}
		}	
	}
}

void AppConfigGet (app_config_t * p_app_config)
{
	memcpy(p_app_config, &app_config, sizeof(app_config_t));
}

