/**
  ******************************************************************************
  * @file           : buttons.c
  * @brief          : Buttons driver implementation
  ******************************************************************************
  */

#include "buttons.h"
#include "string.h"

buttons_state_t 	buttons_state[MAX_BUTTONS_NUM];
button_data_t 		buttons_data[MAX_BUTTONS_NUM/8];
pov_data_t 				pov_data[MAX_POVS_NUM];
uint8_t						pov_pos[MAX_POVS_NUM];
uint8_t 					pos = 0;
pin_config_t * 		p_pin_config;


void ButtonProcess (uint8_t pin_num, app_config_t * p_config)
{	
	uint32_t millis;
	
	// get port state
	buttons_state[pos].pin_prev_state = buttons_state[pos].pin_state;
	buttons_state[pos].pin_state = !HAL_GPIO_ReadPin(p_pin_config[pin_num].port, p_pin_config[pin_num].pin);
	// inverse logic signal
	if (p_config->pins[pin_num] == BUTTON_VCC)
	{
		buttons_state[pos].pin_state = !buttons_state[pos].pin_state;
	}
	// get timestamp
	millis = HAL_GetTick();
	
	// choose config for current button
	switch (p_config->buttons[pos])
	{		
		case BUTTON_INVERTED:
			// invert state for inverted button
			buttons_state[pos].pin_state = !buttons_state[pos].pin_state;						
		case BUTTON_NORMAL:
			
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = buttons_state[pos].pin_state;
				buttons_state[pos].prev_state = buttons_state[pos].current_state;
				buttons_state[pos].cnt += buttons_state[pos].current_state;
			}
			// reset if state changed during debounce period
			else if (	buttons_state[pos].changed && 
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
			}
			break;
			
		case BUTTON_TOGGLE:
			// set timestamp if state changed to HIGH
			if (!buttons_state[pos].changed && 
					buttons_state[pos].pin_state > buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].prev_state = 1;
				buttons_state[pos].current_state = !buttons_state[pos].current_state;
				buttons_state[pos].cnt++;
			}
			// reset if state changed during debounce period
			else if (!buttons_state[pos].pin_state && millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].prev_state = 0;
			}
			break;
			
		case TOGGLE_SWITCH:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = 1;
				buttons_state[pos].prev_state = buttons_state[pos].pin_state;
				buttons_state[pos].cnt++;
			}
			// release button after push time
			else if (	millis - buttons_state[pos].time_last > p_config->toggle_press_time_ms)
			{
				buttons_state[pos].current_state = 0;
				buttons_state[pos].changed = 0;
			}
			break;
		
		case TOGGLE_SWITCH_ON:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state > buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = 1;
				buttons_state[pos].prev_state = buttons_state[pos].pin_state;
				buttons_state[pos].cnt++;
			}
			// release button after push time
			else if (	millis - buttons_state[pos].time_last > p_config->toggle_press_time_ms)
			{
				buttons_state[pos].prev_state = buttons_state[pos].pin_state;
				buttons_state[pos].current_state = 0;
				buttons_state[pos].changed = 0;
			}
			break;
		
		case TOGGLE_SWITCH_OFF:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state < buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = 1;
				buttons_state[pos].prev_state = buttons_state[pos].pin_state;
				buttons_state[pos].cnt++;
			}
			// release button after push time
			else if (	millis - buttons_state[pos].time_last > p_config->toggle_press_time_ms)
			{
				buttons_state[pos].prev_state = buttons_state[pos].pin_state;
				buttons_state[pos].current_state = 0;
				buttons_state[pos].changed = 0;
			}
			break;
			
		case POV1_UP:
		case POV1_RIGHT:
		case POV1_DOWN:
		case POV1_LEFT:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = buttons_state[pos].pin_state;
				buttons_state[pos].prev_state = buttons_state[pos].current_state;
				buttons_state[pos].cnt += buttons_state[pos].current_state;
				
				// set bit in povs data
				if (p_config->buttons[pos] == POV1_UP)
				{
					pov_pos[0] &= !(1 << 3);
					pov_pos[0] |= (buttons_state[pos].current_state << 3);
				}
				else if (p_config->buttons[pos] == POV1_RIGHT)
				{
					pov_pos[0] &= !(1 << 2);
					pov_pos[0] |= (buttons_state[pos].current_state << 2);
				}
				else if (p_config->buttons[pos] == POV1_DOWN)
				{
					pov_pos[0] &= !(1 << 1);
					pov_pos[0] |= (buttons_state[pos].current_state << 1);
				}
				else
				{
					pov_pos[0] &= !(1 << 0);
					pov_pos[0] |= (buttons_state[pos].current_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	buttons_state[pos].changed && 
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms )
			{
				buttons_state[pos].changed = 0;
			}
			break;
			
		case POV2_UP:
		case POV2_RIGHT:
		case POV2_DOWN:
		case POV2_LEFT:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = buttons_state[pos].pin_state;
				buttons_state[pos].prev_state = buttons_state[pos].current_state;
				buttons_state[pos].cnt += buttons_state[pos].current_state;
				
				// set bit in povs data
				if (p_config->buttons[pos] == POV2_UP)
				{
					pov_pos[1] &= !(1 << 3);
					pov_pos[1] |= (buttons_state[pos].current_state << 3);
				}
				else if (p_config->buttons[pos] == POV2_RIGHT)
				{
					pov_pos[1] &= !(1 << 2);
					pov_pos[1] |= (buttons_state[pos].current_state << 2);
				}
				else if (p_config->buttons[pos] == POV2_DOWN)
				{
					pov_pos[1] &= !(1 << 1);
					pov_pos[1] |= (buttons_state[pos].current_state << 1);
				}
				else
				{
					pov_pos[1] &= !(1 << 0);
					pov_pos[1] |= (buttons_state[pos].current_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	buttons_state[pos].changed && 
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms )
			{
				buttons_state[pos].changed = 0;
			}
			break;	

		case POV3_UP:
		case POV3_RIGHT:
		case POV3_DOWN:
		case POV3_LEFT:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = buttons_state[pos].pin_state;
				buttons_state[pos].prev_state = buttons_state[pos].current_state;
				buttons_state[pos].cnt += buttons_state[pos].current_state;
				
				// set bit in povs data
				if (p_config->buttons[pos] == POV3_UP)
				{
					pov_pos[2] &= !(1 << 3);
					pov_pos[2] |= (buttons_state[pos].current_state << 3);
				}
				else if (p_config->buttons[pos] == POV3_RIGHT)
				{
					pov_pos[2] &= !(1 << 2);
					pov_pos[2] |= (buttons_state[pos].current_state << 2);
				}
				else if (p_config->buttons[pos] == POV3_DOWN)
				{
					pov_pos[2] &= !(1 << 1);
					pov_pos[2] |= (buttons_state[pos].current_state << 1);
				}
				else
				{
					pov_pos[2] &= !(1 << 0);
					pov_pos[2] |= (buttons_state[pos].current_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	buttons_state[pos].changed && 
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms )
			{
				buttons_state[pos].changed = 0;
			}
			break;
			
		case POV4_UP:
		case POV4_RIGHT:
		case POV4_DOWN:
		case POV4_LEFT:
			// set timestamp if state changed
			if (!buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state)		
			{
				buttons_state[pos].time_last = millis;
				buttons_state[pos].changed = 1;
			}
			// set state after debounce if state have not changed
			else if (	buttons_state[pos].changed && buttons_state[pos].pin_state != buttons_state[pos].prev_state &&
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms)
			{
				buttons_state[pos].changed = 0;
				buttons_state[pos].current_state = buttons_state[pos].pin_state;
				buttons_state[pos].prev_state = buttons_state[pos].current_state;
				buttons_state[pos].cnt += buttons_state[pos].current_state;
				
				// set bit in povs data
				if (p_config->buttons[pos] == POV4_UP)
				{
					pov_pos[3] &= !(1 << 3);
					pov_pos[3] |= (buttons_state[pos].current_state << 3);
				}
				else if (p_config->buttons[pos] == POV4_RIGHT)
				{
					pov_pos[3] &= !(1 << 2);
					pov_pos[3] |= (buttons_state[pos].current_state << 2);
				}
				else if (p_config->buttons[pos] == POV4_DOWN)
				{
					pov_pos[3] &= !(1 << 1);
					pov_pos[3] |= (buttons_state[pos].current_state << 1);
				}
				else
				{
					pov_pos[3] &= !(1 << 0);
					pov_pos[3] |= (buttons_state[pos].current_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	buttons_state[pos].changed && 
								millis - buttons_state[pos].time_last > p_config->button_debounce_ms )
			{
				buttons_state[pos].changed = 0;
			}
			break;
			
		// TODO: Special button modes (analog, shift, etc.)
		case BUTTON_TO_ANALOG:
			break;
		
		case BUTTON_SHIFT:
			break;
		
		default:
			break;
		
	}
	
}

void ButtonsCheck (app_config_t * p_config)
{
	pos = 0;
	GetPinConfig(&p_pin_config);
	
	// check single buttons
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == BUTTON_GND || 
				p_config->pins[i] == BUTTON_VCC)
		{
			ButtonProcess(i, p_config);
			pos++;
		}
	}
	
	// check matrix buttons
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == BUTTON_COLUMN)
		{
			// tie Column pin to ground
			HAL_GPIO_WritePin(p_pin_config[i].port, p_pin_config[i].pin, GPIO_PIN_RESET);
			
			// check states at Rows
			for (int k=0; k<USED_PINS_NUM; k++)
			{
				if (pos > MAX_BUTTONS_NUM)
				{
					_Error_Handler(__FILE__, __LINE__);
				}
				if (p_config->pins[k] == BUTTON_ROW)
				{ 
					ButtonProcess(k, p_config);
					pos++;
				}
			}
			// return Column pin to Hi-Z state
			HAL_GPIO_WritePin(p_pin_config[i].port, p_pin_config[i].pin, GPIO_PIN_SET);
		}
	}
	
	EncoderProcess(buttons_state, p_config);
	
	// convert data to report format
	for (int i=0;i<pos;i++)
		{
			buttons_data[(i & 0xF8)>>3] &= ~(1 << (i & 0x07));
			buttons_data[(i & 0xF8)>>3] |= (buttons_state[i].current_state << (i & 0x07));
		}
	
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

void ButtonsGet (button_data_t * data)
{
	if (data != NULL)
	{
		memcpy(data, buttons_data, sizeof(buttons_data));
	}
}

void POVsGet (pov_data_t * data)
{
	if (data != NULL)
	{
		memcpy(data, pov_data, sizeof(pov_data));
	}
}



