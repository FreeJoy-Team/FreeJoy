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

uint8_t												raw_buttons_data[MAX_BUTTONS_NUM];
physical_buttons_state_t 			physical_buttons_state[MAX_BUTTONS_NUM];
logical_buttons_state_t 			logical_buttons_state[MAX_BUTTONS_NUM];
button_data_t 								buttons_data[MAX_BUTTONS_NUM/8];
pov_data_t 										pov_data[MAX_POVS_NUM];
uint8_t												pov_pos[MAX_POVS_NUM];
uint8_t												shifts_state;
uint8_t												a2b_first;
uint8_t												a2b_last;

/**
  * @brief  Processing debounce for raw buttons input
	* @param  p_dev_config: Pointer to device configuration
  * @retval None
  */
void ButtonsDebouceProcess (dev_config_t * p_dev_config)
{
	uint32_t 	millis;
	uint16_t	debounce;
	
	millis = GetTick();
	
	for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
	{
			// set a2b debounce
			if (a2b_first != a2b_last && i > a2b_first && i <= a2b_last)
			{
				debounce = p_dev_config->a2b_debounce_ms;
			}
			else 
			{
				debounce = p_dev_config->button_debounce_ms;
			}
		
			physical_buttons_state[i].prev_pin_state = physical_buttons_state[i].pin_state;
			physical_buttons_state[i].pin_state = raw_buttons_data[i];
		
			// set timestamp if state changed
			if (!physical_buttons_state[i].changed && physical_buttons_state[i].pin_state != physical_buttons_state[i].prev_pin_state)		
			{
				physical_buttons_state[i].time_last = millis;
				physical_buttons_state[i].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	physical_buttons_state[i].changed && physical_buttons_state[i].pin_state == physical_buttons_state[i].prev_pin_state &&
								millis - physical_buttons_state[i].time_last > debounce)
			{

				physical_buttons_state[i].changed = 0;
				physical_buttons_state[i].current_state = physical_buttons_state[i].pin_state;
				//physical_buttons_state[i].cnt++;
			}
			// reset if state changed during debounce period
			else if (physical_buttons_state[i].changed &&
								millis - physical_buttons_state[i].time_last > debounce)
			{
				physical_buttons_state[i].changed = 0;
			}
	}
}

static void LogicalButtonProcessTimer (logical_buttons_state_t * p_button_state, uint32_t millis, dev_config_t * p_dev_config, uint8_t num)
{
	uint16_t tmp_press_time;
	uint16_t tmp_delay_time;
	
	// get toggle press timer	
		switch (p_dev_config->buttons[num].press_timer)
		{	
			case BUTTON_TIMER_1:
				tmp_press_time = p_dev_config->button_timer1_ms;
				break;
			case BUTTON_TIMER_2:
				tmp_press_time = p_dev_config->button_timer2_ms;
				break;
			case BUTTON_TIMER_3:
				tmp_press_time = p_dev_config->button_timer3_ms;
				break;
			default:
				tmp_press_time = 100;
				break;
		}
	
	// get delay 	
		if(tmp_press_time <= 0)
		{
			tmp_press_time = 100;
		}
		switch (p_dev_config->buttons[num].delay_timer)
		{	
			case BUTTON_TIMER_1:
				tmp_delay_time = p_dev_config->button_timer1_ms;
				break;
			case BUTTON_TIMER_2:
				tmp_delay_time = p_dev_config->button_timer2_ms;
				break;
			case BUTTON_TIMER_3:
				tmp_delay_time = p_dev_config->button_timer3_ms;
				break;
			default:
				tmp_delay_time = 0;
				break;
		}
		
	// set max delay timer for sequential and radio buttons // heroviy kostil`, need if for check all seq buttons for types of timings
//	if (p_dev_config->buttons[num].delay_timer && 
//		 (p_dev_config->buttons[num].type == SEQUENTIAL_TOGGLE || p_dev_config->buttons[num].type == SEQUENTIAL_BUTTON))
//	{
//		if(p_dev_config->button_timer1_ms > p_dev_config->button_timer2_ms && p_dev_config->button_timer1_ms > p_dev_config->button_timer3_ms)
//				tmp_delay_time = p_dev_config->button_timer1_ms;
//		else if(p_dev_config->button_timer2_ms > p_dev_config->button_timer1_ms && p_dev_config->button_timer2_ms > p_dev_config->button_timer3_ms)
//				tmp_delay_time = p_dev_config->button_timer2_ms;
//		else
//				tmp_delay_time = p_dev_config->button_timer3_ms;
//	}
	
	// check if delay timer elapsed
	if ((p_button_state->delay_act == BUTTON_ACTION_DELAY && millis - p_button_state->time_last > tmp_delay_time &&
						millis - p_button_state->time_last < tmp_press_time + tmp_delay_time))
	{	
		p_button_state->delay_act = BUTTON_ACTION_PRESS;		
	}
	// check if press timer elapsed
	else if ((p_button_state->delay_act == BUTTON_ACTION_PRESS && 
			 millis - p_button_state->time_last > tmp_press_time + tmp_delay_time))
	{
		p_button_state->delay_act = BUTTON_ACTION_IDLE;
	}
}

/**
  * @brief  Getting logical button state accoring to its configuration
  * @param  p_button_state:	Pointer to button state structure
	* @param  pov_buf: Pointer to POV states buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  num: Button number
  * @retval None
  */
void LogicalButtonProcessState (logical_buttons_state_t * p_button_state, uint8_t * pov_buf, dev_config_t * p_dev_config, uint8_t num)
{	
	
	uint32_t millis;
	uint8_t pov_group = 0;
	
	millis = GetTick();
	LogicalButtonProcessTimer(p_button_state, millis, p_dev_config, num);
	
		switch (p_dev_config->buttons[num].type)
		{				
			case BUTTON_NORMAL:
			case POV1_CENTER:
			case POV2_CENTER:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = p_button_state->curr_physical_state;
					//p_button_state->off_state = !p_button_state->on_state;
				}
				else	// IDLE state
				{
					p_button_state->current_state = p_button_state->curr_physical_state;
				}
				break;
				
			case BUTTON_TOGGLE:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = !p_button_state->current_state;
					p_button_state->off_state = p_button_state->on_state;
				}
				else	// IDLE state
				{
					// nop
				}
				break;
				
			case TOGGLE_SWITCH:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state != p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = 1;				// check when inverted!
					p_button_state->off_state = 0;
				}
				else	// IDLE state
				{
					p_button_state->current_state = p_button_state->off_state;
				}
				break;
				
			case TOGGLE_SWITCH_ON:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = 1;				// check when inverted!
					p_button_state->off_state = 0;
				}
				else	// IDLE state
				{
					p_button_state->current_state = p_button_state->off_state;
				}
				break;
			 
			case TOGGLE_SWITCH_OFF:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state < p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = 1;				// check when inverted!
					p_button_state->off_state = 0;
				}
				else	// IDLE state
				{
					p_button_state->current_state = p_button_state->off_state;
				}
				break;
				
			case POV4_UP:
			case POV4_RIGHT:
			case POV4_DOWN:
			case POV4_LEFT:
				pov_group = 3;
			
			case POV3_UP:
			case POV3_RIGHT:
			case POV3_DOWN:
			case POV3_LEFT:	
				if (!pov_group) pov_group = 2;
			
			case POV2_UP:
			case POV2_RIGHT:
			case POV2_DOWN:
			case POV2_LEFT:		
				if (!pov_group) pov_group = 1;
			
			case POV1_UP:
			case POV1_RIGHT:
			case POV1_DOWN:
			case POV1_LEFT:
				if (pov_group<=0) pov_group = 0;
				
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = p_button_state->curr_physical_state;
					p_button_state->off_state = !p_button_state->on_state;
				}
				else	// IDLE state
				{
					//p_button_state->current_state = p_button_state->off_state;
					p_button_state->current_state = p_button_state->curr_physical_state;
				}
					
				// set bit in povs data
				if (p_dev_config->buttons[num].type == POV1_UP || p_dev_config->buttons[num].type == POV2_UP ||
						p_dev_config->buttons[num].type == POV3_UP || p_dev_config->buttons[num].type == POV4_UP)
				{
					pov_buf[pov_group] &= ~(1 << 3);
					pov_buf[pov_group] |= (p_button_state->current_state << 3);
				}
				else if (p_dev_config->buttons[num].type == POV1_RIGHT || p_dev_config->buttons[num].type == POV2_RIGHT ||
								 p_dev_config->buttons[num].type == POV3_RIGHT || p_dev_config->buttons[num].type == POV4_RIGHT)
				{
					pov_buf[pov_group] &= ~(1 << 2);
					pov_buf[pov_group] |= (p_button_state->current_state << 2);
				}
				else if (p_dev_config->buttons[num].type == POV1_DOWN || p_dev_config->buttons[num].type == POV2_DOWN ||
								 p_dev_config->buttons[num].type == POV3_DOWN || p_dev_config->buttons[num].type == POV4_DOWN)
				{
					pov_buf[pov_group] &= ~(1 << 1);
					pov_buf[pov_group] |= (p_button_state->current_state << 1);
				}
				else
				{
					pov_buf[pov_group] &= ~(1 << 0);
					pov_buf[pov_group] |= (p_button_state->current_state << 0);
				}
				
				// turn off POV center button if one of directions is pressed
				if (pov_buf[pov_group] != 0)
				{
					if (pov_group == 0)
					{
						for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)	
						{
							if (p_dev_config->buttons[i].type == POV1_CENTER)	
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_IDLE;
								logical_buttons_state[i].on_state = 0;
								logical_buttons_state[i].off_state = 0;
								logical_buttons_state[i].current_state = 0;
								logical_buttons_state[i].curr_physical_state = 0;
								logical_buttons_state[i].time_last = 0;		 
							}
						}
					}
					else if (pov_group == 1)
					{
						for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)	
						{
							if (p_dev_config->buttons[i].type == POV2_CENTER)	
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_IDLE;
								logical_buttons_state[i].on_state = 0;
								logical_buttons_state[i].off_state = 0;
								logical_buttons_state[i].current_state = 0;
								logical_buttons_state[i].curr_physical_state = 0;
								logical_buttons_state[i].time_last = 0;		 
							}
						}
					}
				}
				
				break;
							
			case RADIO_BUTTON1:
			case RADIO_BUTTON2:
			case RADIO_BUTTON3:
			case RADIO_BUTTON4:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
					
					for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
					{
						if (p_dev_config->buttons[i].type == p_dev_config->buttons[num].type && i != num)
						{
							logical_buttons_state[i].current_state = logical_buttons_state[i].off_state;
						}
					}
				}
				else if (p_button_state->curr_physical_state)// > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					p_button_state->delay_act = BUTTON_ACTION_DELAY;
					p_button_state->time_last = millis;
					p_button_state->on_state = 1;
					p_button_state->off_state = 0;
				}
				else	// IDLE state
				{
					// nop
				}			
				break;
				
			case SEQUENTIAL_TOGGLE:
					if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					// searching for enabled button
					uint8_t is_last = 1;
					uint8_t is_set_found = 0;
					for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
					{
						if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
							p_dev_config->buttons[i].type == SEQUENTIAL_TOGGLE)
						{
							//disable enabled button
							if (logical_buttons_state[i].on_state == 1 && 
									logical_buttons_state[i].delay_act == BUTTON_ACTION_IDLE)	// prevent multiple enabling
							{
								logical_buttons_state[i].on_state = 0;
								logical_buttons_state[i].off_state = 0;
								logical_buttons_state[i].current_state = 0;
								is_set_found = 1;
							}
							else if (is_set_found)	// enable next button in list
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_DELAY;
								logical_buttons_state[i].time_last = millis;
								
								logical_buttons_state[i].on_state = 1;
								logical_buttons_state[i].off_state = 0;
								is_last = 0;
								break;
							}
						}
					}
					
					// previously enabled button was last in list
					// finding first in list and enable it
					if (is_last && is_set_found)
					{
						for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
						{
							if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
								p_dev_config->buttons[i].type == SEQUENTIAL_TOGGLE)
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_DELAY;
								logical_buttons_state[i].time_last = millis;
								
								logical_buttons_state[i].on_state = 1;
								logical_buttons_state[i].off_state = 0;
								break;
							}
						}
					}
					
				}
				else if (!p_button_state->curr_physical_state)	// IDLE state
				{
					// nop
				}	
				
				break;

			case SEQUENTIAL_BUTTON:
				if (p_button_state->delay_act == BUTTON_ACTION_DELAY)
				{
					// nop
				}
				else if (p_button_state->delay_act == BUTTON_ACTION_PRESS)
				{
					p_button_state->current_state = p_button_state->on_state;
				}
				else if (p_button_state->curr_physical_state > p_button_state->prev_physical_state)		// triggered in IDLE
				{
					// searching for enabled button
					uint8_t is_last = 1;
					uint8_t is_set_found = 0;
					for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
					{
						if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
							p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON)
						{
							//disable enabled button
							if (logical_buttons_state[i].on_state == 1 && 
									logical_buttons_state[i].delay_act == BUTTON_ACTION_IDLE)	// prevent multiple enabling
							{
								logical_buttons_state[i].on_state = 0;
								logical_buttons_state[i].off_state = 0;
								is_set_found = 1;
							}
							else if (is_set_found)	// enable next button in list
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_DELAY;
								logical_buttons_state[i].time_last = millis;
								
								logical_buttons_state[i].on_state = 1;
								logical_buttons_state[i].off_state = 0;
								is_last = 0;
								break;
							}
						}
					}
					
					// previously enabled button was last in list
					// finding first in list and enable it
					if (is_last && is_set_found)
					{
						for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
						{
							if (p_dev_config->buttons[i].physical_num == p_dev_config->buttons[num].physical_num &&
								p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON)
							{
								logical_buttons_state[i].delay_act = BUTTON_ACTION_DELAY;
								logical_buttons_state[i].time_last = millis;
								
								logical_buttons_state[i].on_state = 1;
								logical_buttons_state[i].off_state = 0;
								break;
							}
						}
					}
					
				}
				else if (!p_button_state->curr_physical_state)	// IDLE state
				{
					p_button_state->current_state = p_button_state->off_state;
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
				logical_buttons_state[j].on_state = 1;
				logical_buttons_state[j].off_state = 0;
				logical_buttons_state[j].current_state = logical_buttons_state[j].on_state;
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
	// enable first
	for (uint8_t physical_num=0; physical_num<MAX_BUTTONS_NUM; physical_num++)
	{
		for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
		{
			if (p_dev_config->buttons[i].type == SEQUENTIAL_TOGGLE &&
					p_dev_config->buttons[i].physical_num == physical_num)
			{
				logical_buttons_state[i].on_state = 1;
				logical_buttons_state[i].current_state = 1;
				break;
			}
		}
	}
	// enable last
//	for (uint8_t physical_num=0; physical_num<MAX_BUTTONS_NUM; physical_num++)
//	{
//		uint8_t k=0;
//		for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
//		{
//			if (p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON &&
//					p_dev_config->buttons[i].physical_num == physical_num)
//			{
//				k++;
//			}
//		}
//		if (k>0) logical_buttons_state[k-1].on_state = 1;
//		
//	}
	
		for (int8_t physical_num = MAX_BUTTONS_NUM - 1; physical_num > -1; physical_num--)
	{
		for (int8_t i = MAX_BUTTONS_NUM - 1; i > -1; i--)
		{
			if (p_dev_config->buttons[i].type == SEQUENTIAL_BUTTON &&
					p_dev_config->buttons[i].physical_num != physical_num)
			{
				logical_buttons_state[i].on_state = 1;
				//buttons_state[i].current_state = 1;
				//buttons_state[i].prev_state = 1;
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

/**
  * @brief  Getting buttons states of single buttons
	* @param  p_dev_config: Pointer to device configuration
	* @param  p_buf: Pointer to raw buttons buffer
  * @retval Buttons count
  */
uint8_t ButtonsReadPhysical(dev_config_t * p_dev_config, uint8_t * p_buf)
{
	uint8_t pos = 0;

	// Getting physical buttons states
	MaxtrixButtonsGet(p_buf, p_dev_config, &pos);
	ShiftRegistersGet(p_buf, p_dev_config, &pos);
	a2b_first = pos;
	AxesToButtonsGet(p_buf, p_dev_config, &pos);
	a2b_last = pos;
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
	// Process regular buttons
	for (uint8_t i=0; i<MAX_BUTTONS_NUM; i++)
	{
		uint8_t shift_num = 0;
		
		// check logical buttons to have shift modificators							// disable if no shift?
		if (p_dev_config->shift_config[0].button >=0 ||										// or shift_config[*].button ?
				p_dev_config->shift_config[1].button >=0 ||
				p_dev_config->shift_config[2].button >=0 ||
				p_dev_config->shift_config[3].button >=0 ||
				p_dev_config->shift_config[4].button >=0)	
		{
			for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
			{
				int8_t btn = p_dev_config->buttons[j].physical_num;
				
				if (btn == i && (p_dev_config->buttons[j].shift_modificator))				// we found button this shift modificator 
				{
					shift_num = p_dev_config->buttons[j].shift_modificator;
					if (shifts_state & 1<<(shift_num-1))											// shift pressed for this button
					{
						logical_buttons_state[j].prev_physical_state = logical_buttons_state[j].curr_physical_state;
						logical_buttons_state[j].curr_physical_state = physical_buttons_state[p_dev_config->buttons[j].physical_num].current_state;
						
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}
					else if (logical_buttons_state[j].current_state)	// shift released for this button
					{
						// disable button
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;
						logical_buttons_state[j].curr_physical_state = 0;
						logical_buttons_state[j].time_last = 0;			
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}
				}
			}
		}
		
		if (shift_num == 0)		// we found not shift modificated physical button
		{
			for (uint8_t j=0; j<MAX_BUTTONS_NUM; j++)
			{
				if (p_dev_config->buttons[j].physical_num == i)		// we found corresponding logical button
				{
					logical_buttons_state[j].prev_physical_state = logical_buttons_state[j].curr_physical_state;
					logical_buttons_state[j].curr_physical_state = physical_buttons_state[p_dev_config->buttons[j].physical_num].current_state;		
					
					LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
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
					logical_buttons_state[j].prev_physical_state = logical_buttons_state[j].curr_physical_state;
					logical_buttons_state[j].curr_physical_state = physical_buttons_state[p_dev_config->buttons[j].physical_num].current_state;
					
					LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
				}
				// shift pressed
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<0) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (logical_buttons_state[j].current_state)	
					{
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;
						logical_buttons_state[j].curr_physical_state = 0;
						logical_buttons_state[j].time_last = 0;
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}	
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<1)	&&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (logical_buttons_state[j].current_state)	
					{
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;	
						logical_buttons_state[j].curr_physical_state = 0;
						logical_buttons_state[j].time_last = 0;						
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}		
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<2) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (logical_buttons_state[j].current_state)	
					{
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;	
						logical_buttons_state[j].curr_physical_state = 0;						
						logical_buttons_state[j].time_last = 0;						
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}	
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<3) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (logical_buttons_state[j].current_state)	
					{
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;
						logical_buttons_state[j].curr_physical_state = 0;
						logical_buttons_state[j].time_last = 0;									
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
					}		
				}
				else if (p_dev_config->buttons[j].physical_num == i && shifts_state & (1<<4) &&
					(p_dev_config->buttons[j].shift_modificator) == 0)
				{
					// disable button
					if (logical_buttons_state[j].current_state)	
					{
						logical_buttons_state[j].delay_act = BUTTON_ACTION_IDLE;
						logical_buttons_state[j].on_state = 0;
						logical_buttons_state[j].off_state = 0;
						logical_buttons_state[j].current_state = 0;
						logical_buttons_state[j].curr_physical_state = 0;
						logical_buttons_state[j].time_last = 0;									
						LogicalButtonProcessState(&logical_buttons_state[j], pov_pos, p_dev_config, j);
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
					shifts_state |= (logical_buttons_state[j].current_state << i);
				}
			}
		}
	}
	
	// convert data to report format	
	uint8_t k = 0;
	for (int i=0;i<MAX_BUTTONS_NUM;i++)
	{
			if (!p_dev_config->buttons[i].is_disabled && p_dev_config->buttons[i].physical_num >= 0)
			{
				// prevent not atomic read
				NVIC_DisableIRQ(TIM2_IRQn);
				
				buttons_data[(k & 0xF8)>>3] &= ~(1 << (k & 0x07));
				if (!p_dev_config->buttons[i].is_inverted)
				{					
					buttons_data[(k & 0xF8)>>3] |= (logical_buttons_state[i].current_state << (k & 0x07));
				}
				else
				{
					buttons_data[(k & 0xF8)>>3] |= (!logical_buttons_state[i].current_state << (k & 0x07));
				}
				k++;
				// resume IRQ
				NVIC_EnableIRQ(TIM2_IRQn);
			}
	}
	
	// convert POV data to report format
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



