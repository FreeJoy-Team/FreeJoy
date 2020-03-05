/**
  ******************************************************************************
  * @file           : config.c
  * @brief          : Config management implementation
		
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

#include "config.h"

app_config_t app_config;

uint8_t DevConfigCheck (dev_config_t * p_dev_config)
{
	uint8_t ret = 0;
	
	if (p_dev_config == NULL)	ret = 1;
	
	return ret;
}

void DevConfigSet (dev_config_t * p_dev_config)
{
	uint32_t data_addr = (uint32_t) p_dev_config;
	uint32_t prog_addr;
	
	if (DevConfigCheck(p_dev_config) != 0)
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
	app_config.axes = 0;
	app_config.buttons_cnt = 0;
	app_config.povs = 0;
	
	if (p_dev_config->is_dynamic_config)
	{
		for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
		{
			if (p_dev_config->axis_config[i].out_enabled)	app_config.axes |= (1<<i);
		}

		for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
		{
			uint8_t is_hidden = 0;
			
			if (i == p_dev_config->shift_config[0].button ||
						i == p_dev_config->shift_config[1].button ||
						i == p_dev_config->shift_config[2].button ||
						i == p_dev_config->shift_config[3].button ||
						i == p_dev_config->shift_config[4].button)	continue;
			
			if (p_dev_config->buttons[i].type == POV1_DOWN ||
						p_dev_config->buttons[i].type == POV1_UP ||
						p_dev_config->buttons[i].type == POV1_LEFT ||
						p_dev_config->buttons[i].type == POV1_RIGHT)
			{
				app_config.povs |= 0x01;
				continue;
			}
			if (p_dev_config->buttons[i].type == POV2_DOWN ||
						p_dev_config->buttons[i].type == POV2_UP ||
						p_dev_config->buttons[i].type == POV2_LEFT ||
						p_dev_config->buttons[i].type == POV2_RIGHT)
			{
				app_config.povs |= 0x02;
				continue;
			}
			if (p_dev_config->buttons[i].type == POV3_DOWN ||
						p_dev_config->buttons[i].type == POV3_UP ||
						p_dev_config->buttons[i].type == POV3_LEFT ||
						p_dev_config->buttons[i].type == POV3_RIGHT)
			{
				app_config.povs |= 0x04;
				continue;
			}
			if (p_dev_config->buttons[i].type == POV4_DOWN ||
						p_dev_config->buttons[i].type == POV4_UP ||
						p_dev_config->buttons[i].type == POV4_LEFT ||
						p_dev_config->buttons[i].type == POV4_RIGHT)
			{
				app_config.povs |= 0x08;
				continue;
			}

			for (uint8_t j=0; j<MAX_AXIS_NUM; j++)
			{
					// button is mapped to axis
					if (i == p_dev_config->axis_config[j].decrement_button ||
							i == p_dev_config->axis_config[j].increment_button)
					{
						is_hidden = 1;
						break;
					}
			}
			
			if (!is_hidden && p_dev_config->buttons[i].physical_num >=0)
			{
				app_config.buttons_cnt++;
			}
		}		
	}
	else
	{
		app_config.axes = 0xFF;
		app_config.buttons_cnt = MAX_BUTTONS_NUM;
		app_config.povs = 0x0F;
	}
	
}

void AppConfigGet (app_config_t * p_app_config)
{
	memcpy(p_app_config, &app_config, sizeof(app_config_t));
}

