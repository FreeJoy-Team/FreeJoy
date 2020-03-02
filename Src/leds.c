/**
  ******************************************************************************
  * @file           : leds.c
  * @brief          : LEDs driver implementation
		
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
	
#include "leds.h"
#include "buttons.h"
	
uint8_t leds_state[MAX_LEDS_NUM];
	
void LEDs_LogicalProcess (dev_config_t * p_dev_config)
{
	for (uint8_t i=0; i<MAX_LEDS_NUM; i++)
	{
		if (p_dev_config->leds[i].input_num >= 0)
		{
			switch (p_dev_config->leds[i].type)
			{
				default:
				case LED_NORMAL:
					leds_state[i] = buttons_state[p_dev_config->leds[i].input_num].current_state;
				break;
				
				case LED_INVERTED:
					leds_state[i] = !buttons_state[p_dev_config->leds[i].input_num].current_state;
				break;
				
			}
		}
	}
}

void LED_SetSingle(uint8_t * state_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	for (uint8_t i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == LED_SINGLE)
		{
			leds_state[(*pos)] ? (pin_config[i].port->ODR |= pin_config[i].pin) : (pin_config[i].port->ODR &= !pin_config[i].pin); 
			(*pos)++;
		}
	}
}

void LED_SetMatrix(uint8_t * state_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	for (uint8_t i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == LED_COLUMN)
		{
			pin_config[i].port->ODR |= pin_config[i].pin;
			for (uint8_t j=0; j<USED_PINS_NUM; j++)
			{
				if (p_dev_config->pins[i] == LED_ROW)
				{
					leds_state[(*pos)] ? (pin_config[j].port->ODR &= ~pin_config[j].pin) : (pin_config[j].port->ODR |= pin_config[j].pin); 
					(*pos)++;				
				}
			}
			pin_config[i].port->ODR &= ~pin_config[i].pin;			
		}
	}
}

void LEDs_PhysicalProcess (dev_config_t * p_dev_config)
{
	uint8_t pos = 0;
	
	LEDs_LogicalProcess(p_dev_config);
	
	LED_SetMatrix(leds_state, p_dev_config, &pos);
	LED_SetSingle(leds_state, p_dev_config, &pos);
		
	
}


