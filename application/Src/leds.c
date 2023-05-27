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
static int32_t time_last[4];
	
void LEDs_LogicalProcess (dev_config_t * p_dev_config)
{
	int32_t millis = GetMillis();
	int8_t input_num = -1;
	
	for (uint8_t i=0; i<MAX_LEDS_NUM; i++)
	{
		input_num = p_dev_config->leds[i].input_num;
		if (input_num >= 0)
		{
			uint8_t but_state = logical_buttons_state[input_num].current_state;
			if (p_dev_config->buttons[input_num].is_inverted)
			{
				but_state = !but_state;
			}
			
			switch (p_dev_config->leds[i].type)
			{
				default:
					
				case LED_NORMAL:
					if (p_dev_config->leds[i].timer == -1 || !but_state)
					{
						leds_state[i] = but_state;
					}
					else if (but_state)
					{
						for (int j = 0; j < 4; j++)
						{
							if (p_dev_config->leds[i].timer == j)
							{
								if (millis - time_last[j] > p_dev_config->led_timer_ms[j])
								{
									leds_state[i] = !leds_state[i];
									time_last[j] = millis;
								}
							}
						}
					}
				break;
				
				case LED_INVERTED:
					if (p_dev_config->leds[i].timer == -1 || but_state)
					{
						leds_state[i] = !but_state;
					}
					else if (!but_state)
					{
						for (int j = 0; j < 4; j++)
						{
							if (p_dev_config->leds[i].timer == j)
							{
								if (millis - time_last[j] > p_dev_config->led_timer_ms[j])
								{
									leds_state[i] = !leds_state[i];
									time_last[j] = millis;
								}
							}
						}
					}
				
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
			leds_state[*pos] ? (pin_config[i].port->ODR |= pin_config[i].pin) : (pin_config[i].port->ODR &= ~pin_config[i].pin); 
			(*pos)++;
		}
	}
}

void LED_SetMatrix(uint8_t * state_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	static int8_t last_row = -1;
	static uint8_t last_pos = 0;
	int8_t max_row = -1;
	
	for (uint8_t i=0; i<USED_PINS_NUM; i++)
	{
		// turn off leds
		if (p_dev_config->pins[i] == LED_ROW)
		{
			pin_config[i].port->ODR &= ~pin_config[i].pin;
			max_row = i;
			for (uint8_t j=0; j<USED_PINS_NUM; j++)
			{
				if (p_dev_config->pins[j] == LED_COLUMN)
				{
					pin_config[j].port->ODR |= pin_config[j].pin;
					(*pos)++;
				}
			}
		}
	}
	for (uint8_t i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == LED_ROW && (i > last_row || i == max_row))
		{
			pin_config[i].port->ODR |= pin_config[i].pin;
			for (uint8_t j=0; j<USED_PINS_NUM; j++)
			{
				if (p_dev_config->pins[j] == LED_COLUMN)
				{
					if (leds_state[last_pos++] > 0) 
					{
						pin_config[j].port->ODR &= ~pin_config[j].pin;
					}
					else
					{
						pin_config[j].port->ODR |= pin_config[j].pin; 
					}				
				}
			}
			if (last_pos >= *pos) last_pos = 0;
			(i == max_row) ? (last_row = -1): (last_row = i);
			break;
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


