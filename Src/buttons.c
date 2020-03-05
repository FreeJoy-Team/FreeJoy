/**
  ******************************************************************************
  * @file           : buttons.c
  * @brief          : Buttons driver implementation
		
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

#include "buttons.h"
#include "string.h"

buttons_state_t 	buttons_state[MAX_BUTTONS_NUM];
button_data_t 		buttons_data[MAX_BUTTONS_NUM/8];
pov_data_t 				pov_data[MAX_POVS_NUM];
uint8_t						pov_pos[MAX_POVS_NUM];
uint8_t						raw_buttons_data[MAX_BUTTONS_NUM];
uint8_t						shifts_state;					

/**
  * @brief  Getting logical button state accoring to its configuration
  * @param  p_button_state:	Pointer to button state structure
	* @param  pov_buf: Pointer to POV states buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  num: Button number
  * @retval None
  */
void LogicalButtonProcessState (buttons_state_t * p_button_state, uint8_t * pov_buf, dev_config_t * p_dev_config, uint8_t num)
{
	uint32_t 	millis;
	
	millis = GetTick();
	// choose config for current button
	switch (p_dev_config->buttons[num].type)
	{		
		case BUTTON_INVERTED:
			// invert state for inverted button
			p_button_state->pin_state = !p_button_state->pin_state;						
		case BUTTON_NORMAL:
			
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->current_state;
				p_button_state->cnt += p_button_state->current_state;
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
			}
			break;
			
		case BUTTON_TOGGLE:
			// set timestamp if state changed to HIGH
			if (!p_button_state->changed && 
					p_button_state->pin_state > p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 1;
				p_button_state->current_state = !p_button_state->current_state;
				p_button_state->cnt++;
			}
			// reset if state changed during debounce period
			else if (!p_button_state->pin_state && millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 0;
			}
			break;
			
		case TOGGLE_SWITCH:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_dev_config->toggle_press_time_ms)
			{
				p_button_state->current_state = 0;
				p_button_state->changed = 0;
			}
			break;
		
		case TOGGLE_SWITCH_ON:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state > p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_dev_config->toggle_press_time_ms)
			{
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->current_state = 0;
				p_button_state->changed = 0;
			}
			break;
		
		case TOGGLE_SWITCH_OFF:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state < p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_dev_config->toggle_press_time_ms)
			{
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->current_state = 0;
				p_button_state->changed = 0;
			}
			break;
			
		case POV1_UP:
		case POV1_RIGHT:
		case POV1_DOWN:
		case POV1_LEFT:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if ((p_dev_config->buttons[num].type) == POV1_UP)
				{
					pov_buf[0] &= ~(1 << 3);
					pov_buf[0] |= (p_button_state->pin_state << 3);
				}
				else if ((p_dev_config->buttons[num].type) == POV1_RIGHT)
				{
					pov_buf[0] &= ~(1 << 2);
					pov_buf[0] |= (p_button_state->pin_state << 2);
				}
				else if ((p_dev_config->buttons[num].type) == POV1_DOWN)
				{
					pov_buf[0] &= ~(1 << 1);
					pov_buf[0] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[0] &= ~(1 << 0);
					pov_buf[0] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms )
			{
				p_button_state->changed = 0;
			}
			break;
			
		case POV2_UP:
		case POV2_RIGHT:
		case POV2_DOWN:
		case POV2_LEFT:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if ((p_dev_config->buttons[num].type) == POV2_UP)
				{
					pov_buf[1] &= ~(1 << 3);
					pov_buf[1] |= (p_button_state->pin_state << 3);
				}
				else if ((p_dev_config->buttons[num].type) == POV2_RIGHT)
				{
					pov_buf[1] &= ~(1 << 2);
					pov_buf[1] |= (p_button_state->pin_state << 2);
				}
				else if ((p_dev_config->buttons[num].type) == POV2_DOWN)
				{
					pov_buf[1] &= ~(1 << 1);
					pov_buf[1] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[1] &= ~(1 << 0);
					pov_buf[1] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms )
			{
				p_button_state->changed = 0;
			}
			break;	

		case POV3_UP:
		case POV3_RIGHT:
		case POV3_DOWN:
		case POV3_LEFT:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if ((p_dev_config->buttons[num].type) == POV3_UP)
				{
					pov_buf[2] &= ~(1 << 3);
					pov_buf[2] |= (p_button_state->pin_state << 3);
				}
				else if ((p_dev_config->buttons[num].type) == POV3_RIGHT)
				{
					pov_buf[2] &= ~(1 << 2);
					pov_buf[2] |= (p_button_state->pin_state << 2);
				}
				else if ((p_dev_config->buttons[num].type) == POV3_DOWN)
				{
					pov_buf[2] &= ~(1 << 1);
					pov_buf[2] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[2] &= ~(1 << 0);
					pov_buf[2] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms )
			{
				p_button_state->changed = 0;
			}
			break;
			
		case POV4_UP:
		case POV4_RIGHT:
		case POV4_DOWN:
		case POV4_LEFT:
			// set timestamp if state changed
			if (!p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state != p_button_state->prev_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if ((p_dev_config->buttons[num].type) == POV4_UP)
				{
					pov_buf[3] &= ~(1 << 3);
					pov_buf[3] |= (p_button_state->pin_state << 3);
				}
				else if ((p_dev_config->buttons[num].type) == POV4_RIGHT)
				{
					pov_buf[3] &= ~(1 << 2);
					pov_buf[3] |= (p_button_state->pin_state << 2);
				}
				else if ((p_dev_config->buttons[num].type) == POV4_DOWN)
				{
					pov_buf[3] &= ~(1 << 1);
					pov_buf[3] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[3] &= ~(1 << 0);
					pov_buf[3] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms )
			{
				p_button_state->changed = 0;
			}
			break;
			
		case RADIO_BUTTON1:
		case RADIO_BUTTON2:
		case RADIO_BUTTON3:
		case RADIO_BUTTON4:
			// set timestamp if state changed to HIGH
			if (!p_button_state->changed && 
					p_button_state->pin_state > p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 1;
				p_button_state->current_state = 1;
				p_button_state->cnt++;
				
				for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
				{
					if (p_dev_config->buttons[i].type == p_dev_config->buttons[num].type && i != num)
					{
						buttons_state[i].current_state = 0;
					}
				}
			}
			// reset if state changed during debounce period
			else if (!p_button_state->pin_state && millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 0;
			}
			break;
			
		case SEQUENTIAL_BUTTON:
			// set timestamp if state changed to HIGH
			if (!p_button_state->changed && 
					p_button_state->pin_state > p_button_state->prev_state)		
			{
				p_button_state->time_last = millis;
				p_button_state->changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	p_button_state->changed && p_button_state->pin_state &&
								millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				uint8_t is_first = 1;
				p_button_state->changed = 0;
				//p_button_state->cnt++;
				
				for (int16_t i=num-1; i>=0; i--)
				{
					if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
							p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON)														
					{
						is_first = 0;
						if (buttons_state[i].current_state && !buttons_state[i].prev_state)
						{
							buttons_state[i].current_state = 0;
							p_button_state->current_state = 1;
							p_button_state->prev_state = p_button_state->pin_state;
							p_button_state->time_last = millis;
							break;
						}
					}
				}
				
				if (is_first && !p_button_state->current_state) // first in list and not pressed
				{
					// search for last in list
					for (int16_t i=MAX_BUTTONS_NUM; i>num; i--)
					{
						// check last
						if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
								p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON)
						{
							if (!buttons_state[i].current_state) break;
							else if (!buttons_state[i].prev_state)
							{
								buttons_state[i].current_state = 0;
								p_button_state->current_state = 1;
								p_button_state->prev_state = p_button_state->pin_state;
								p_button_state->time_last = millis;
								break;
							}
						}	
					}				
				}
				
			}
			// reset if state changed during debounce period
			else if (!p_button_state->pin_state && millis - p_button_state->time_last > p_dev_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 0;
			}	
			break;
		
		default:
			break;
		
	}
}

/**
  * @brief  Set initial states for radio buttons
	* @param  p_dev_config: Pointer to device configuration
  * @retval None
  */
void RadioButtons_Init (dev_config_t * p_dev_config)
{
	for (uint8_t i=0; i<4; i++)
	{
		for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
		{
			if (p_dev_config->buttons[j].type == (RADIO_BUTTON1 + i))
			{
				buttons_state[j].current_state = 1;
				break;
			}
		}
	}
}

/**
  * @brief  Set initial states for sequential buttons
	* @param  p_dev_config: Pointer to device configuration
  * @retval None
  */
void SequentialButtons_Init (dev_config_t * p_dev_config)
{
	for (uint8_t physical_num=0; physical_num<MAX_BUTTONS_NUM; physical_num++)
	{
		for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
		{
			if (p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON &&
					p_dev_config->buttons[i].physical_num == physical_num)
			{
				buttons_state[i].current_state = 1;
				break;
			}
		}
	}
}

/**
  * @brief  Checking single button state
  * @param  pin_num:	Number of pin where button is connected
	* @param  p_dev_config: Pointer to device configuration
  * @retval Buttons state
  */
uint8_t DirectButtonGet (uint8_t pin_num,  dev_config_t * p_dev_config)
{	
	if (p_dev_config->pins[pin_num] == BUTTON_VCC)
	{
		return GPIO_ReadInputDataBit(pin_config[pin_num].port, pin_config[pin_num].pin);
	}
	else 
	{
		return !GPIO_ReadInputDataBit(pin_config[pin_num].port, pin_config[pin_num].pin);
	}	
}

/**
  * @brief  Getting buttons states of matrix buttons
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void MaxtrixButtonsGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	// get matrix buttons
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if ((p_dev_config->pins[i] == BUTTON_ROW) && ((*pos) < MAX_BUTTONS_NUM))
		{
			// tie Row pin to ground
			GPIO_WriteBit(pin_config[i].port, pin_config[i].pin, Bit_RESET);
			
			// get states at Columns
			for (int k=0; k<USED_PINS_NUM; k++)
			{
				if (p_dev_config->pins[k] == BUTTON_COLUMN && (*pos) < MAX_BUTTONS_NUM)
				{ 
					raw_button_data_buf[*pos] = DirectButtonGet(k, p_dev_config);
					(*pos)++;
				}
			}
			// return Row pin to Hi-Z state
			GPIO_WriteBit(pin_config[i].port, pin_config[i].pin, Bit_SET);
		}
	}
}

/**
  * @brief  Getting buttons states of single buttons
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void SingleButtonsGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == BUTTON_GND || 
				p_dev_config->pins[i] == BUTTON_VCC)
		{
			if ((*pos) < MAX_BUTTONS_NUM)
			{
				raw_button_data_buf[*pos] = DirectButtonGet(i, p_dev_config);
				(*pos)++;
			}
			else break;
		}
	}
}


uint8_t ButtonsReadPhysical(dev_config_t * p_dev_config, uint8_t * p_buf)
{
	uint8_t pos = 0;
	// Getting physical buttons states
	MaxtrixButtonsGet(p_buf, p_dev_config, &pos);
	ShiftRegistersGet(p_buf, p_dev_config, &pos);
	AxesToButtonsGet(p_buf, p_dev_config, &pos);
	SingleButtonsGet(p_buf, p_dev_config, &pos);
	
	return pos;
}

/**
  * @brief  Checking all buttons routine
	* @param  p_dev_config: Pointer to device configuration
  * @retval None
  */
void ButtonsReadLogical (dev_config_t * p_dev_config)
{
	uint8_t pos = 0;
	
	pos = ButtonsReadPhysical(p_dev_config, raw_buttons_data);

	// Process regular buttons
	for (uint8_t i=0; i<pos; i++)
	{
		uint8_t shift_num = 0;
		
		// check logical buttons to have shift modificators
		for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
		{
			int8_t btn = p_dev_config->buttons[j].physical_num;
			
			if (btn == i && (p_dev_config->buttons[j].shift_modificator))				// we found button this shift modificator 
			{
				shift_num = p_dev_config->buttons[j].shift_modificator;
				if (shifts_state & 1<<(shift_num-1))											// shift pressed for this button
				{
					buttons_state[j].pin_state = raw_buttons_data[p_dev_config->buttons[j].physical_num];					
					LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
				}
				else if (buttons_state[j].current_state)	// shift released for this button
				{
					// disable button
					buttons_state[j].prev_state = buttons_state[j].pin_state;
					buttons_state[j].pin_state = !buttons_state[j].prev_state;			
					buttons_state[j].changed = 1;
					buttons_state[j].time_last = 0;			
					LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
				}
			}				
		}
		
		if (shift_num == 0)		// we found not shift modificated physical button
		{
			for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
			{
				
				if (p_dev_config->buttons[j].physical_num == i)		// we found corresponding logical button
				{
					buttons_state[j].pin_state = raw_buttons_data[i];				
					LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
				}
			}
		}		
		else	// check if shift is released for modificated physical button
		{
			for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
			{
				if (p_dev_config->buttons[j].physical_num == i && (shifts_state) == 0 &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					buttons_state[j].pin_state = raw_buttons_data[i];				
					LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
				}
				// shift pressed
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<0) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (buttons_state[j].current_state)	
					{
						buttons_state[j].prev_state = buttons_state[j].pin_state;
						buttons_state[j].pin_state = !buttons_state[j].prev_state;	
						buttons_state[j].changed = 1;	
						buttons_state[j].time_last = 0;
						LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
					}	
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<1)	&&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (buttons_state[j].current_state)	
					{
						buttons_state[j].prev_state = buttons_state[j].pin_state;
						buttons_state[j].pin_state = !buttons_state[j].prev_state;	
						buttons_state[j].changed = 1;	
						buttons_state[j].time_last = 0;						
						LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
					}		
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<2) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (buttons_state[j].current_state)	
					{
						buttons_state[j].prev_state = buttons_state[j].pin_state;
						buttons_state[j].pin_state = !buttons_state[j].prev_state;	
						buttons_state[j].changed = 1;		
						buttons_state[j].time_last = 0;						
						LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
					}	
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<3) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (buttons_state[j].current_state)	
					{
						buttons_state[j].prev_state = buttons_state[j].pin_state;
						buttons_state[j].pin_state = !buttons_state[j].prev_state;	
						buttons_state[j].changed = 1;	
						buttons_state[j].time_last = 0;									
						LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
					}		
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<4) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (buttons_state[j].current_state)	
					{
						buttons_state[j].prev_state = buttons_state[j].pin_state;
						buttons_state[j].pin_state = !buttons_state[j].prev_state;	
						buttons_state[j].changed = 1;	
						buttons_state[j].time_last = 0;									
						LogicalButtonProcessState(&buttons_state[j], pov_pos, p_dev_config, j);
					}	
				}
			}
		}
	}	
	
	shifts_state = 0;
	for (uint8_t i=0; i<5; i++)
	{
		if (p_dev_config->shift_config[i].button >= 0)
		{
			for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
			{		
				if (j == p_dev_config->shift_config[i].button)
				{									
					shifts_state |= (buttons_state[j].current_state << i);
				}
			}
		}
	}
	
	// convert data to report format	
	uint8_t k = 0;
	for (int i=0;i<MAX_BUTTONS_NUM;i++)
	{
			uint8_t is_hidden = 0;
			
			// buttons is mapped to shift
			if (i == p_dev_config->shift_config[0].button ||
					i == p_dev_config->shift_config[1].button ||
					i == p_dev_config->shift_config[2].button ||
					i == p_dev_config->shift_config[3].button ||
					i == p_dev_config->shift_config[4].button)	continue;
			
			if (p_dev_config->buttons[i].type == POV1_DOWN ||
					p_dev_config->buttons[i].type == POV1_UP ||
					p_dev_config->buttons[i].type == POV1_LEFT ||
					p_dev_config->buttons[i].type == POV1_RIGHT ||
					p_dev_config->buttons[i].type == POV2_DOWN ||
					p_dev_config->buttons[i].type == POV2_UP ||
					p_dev_config->buttons[i].type == POV2_LEFT ||
					p_dev_config->buttons[i].type == POV2_RIGHT ||
					p_dev_config->buttons[i].type == POV3_DOWN ||
					p_dev_config->buttons[i].type == POV3_UP ||
					p_dev_config->buttons[i].type == POV3_LEFT ||
					p_dev_config->buttons[i].type == POV3_RIGHT ||
					p_dev_config->buttons[i].type == POV4_DOWN ||
					p_dev_config->buttons[i].type == POV4_UP ||
					p_dev_config->buttons[i].type == POV4_LEFT ||
					p_dev_config->buttons[i].type == POV4_RIGHT) continue;	
			
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

			if (!is_hidden)
			{
				// prevent not atomic read
				NVIC_DisableIRQ(TIM1_UP_IRQn);
				
				buttons_data[(k & 0xF8)>>3] &= ~(1 << (k & 0x07));
				buttons_data[(k & 0xF8)>>3] |= (buttons_state[i].current_state << (k & 0x07));
				k++;
				
				// resume IRQ
				NVIC_EnableIRQ(TIM1_UP_IRQn);
			}
	}
	
	// convert encoders data to report format
	for (int i=0; i<MAX_POVS_NUM; i++)
	{
		switch (pov_pos[i])
		{
			case 1:
				pov_data[i] = 0x06;
				break;
			case 2:
				pov_data[i] = 0x04;
				break;
			case 3:
				pov_data[i] = 0x05;
				break;
			case 4:
				pov_data[i] = 0x02;
				break;
			case 6:
				pov_data[i] = 0x03;
				break;
			case 8:
				pov_data[i] = 0x00;
				break;
			case 9:
				pov_data[i] = 0x07;
				break;
			case 12:
				pov_data[i] = 0x01;
				break;
			default:
				pov_data[i] = 0xFF;
				break;
		}
	}
}

/**
  * @brief  Getting buttons data in report format
	* @param  raw_data: Pointer to target buffer of physical buttons
	* @param  data: Pointer to target buffer of logical buttons
  * @retval None
  */
void ButtonsGet (uint8_t * raw_data, button_data_t * data, uint8_t * shift_data)
{
	if (raw_data != NULL)
	{
		memcpy(raw_data, raw_buttons_data, sizeof(raw_buttons_data));
	}
	if (data != NULL)
	{
		memcpy(data, buttons_data, sizeof(buttons_data));
	}
	if (data != NULL)
	{
		memcpy(shift_data, &shifts_state, sizeof(shifts_state));
	}
}
/**
  * @brief  Getting POV data in report format
	* @param  data: Pointer to target buffer
  * @retval None
  */
void POVsGet (pov_data_t * data)
{
	if (data != NULL)
	{
		memcpy(data, pov_data, sizeof(pov_data));
	}
}



