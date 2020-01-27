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
uint8_t						raw_buttons_data[MAX_BUTTONS_NUM];

/**
  * @brief  Getting button state accoring to its configuration
  * @param  p_button_state:	Pointer to button state structure
	* @param  pov_buf: Pointer to POV states buffer
	* @param  p_config: Pointer to device configuration
	* @param  pos: Pointer to button counter variable
  * @retval None
  */
void ButtonProcessState (buttons_state_t * p_button_state, uint8_t * pov_buf, app_config_t * p_config, uint8_t * pos)
{
	uint32_t 	millis;
	
	millis = GetTick();
	// choose config for current button
	switch (p_config->buttons[*pos])
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->current_state;
				p_button_state->cnt += p_button_state->current_state;
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_config->button_debounce_ms)
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->prev_state = 1;
				p_button_state->current_state = !p_button_state->current_state;
				p_button_state->cnt++;
			}
			// reset if state changed during debounce period
			else if (!p_button_state->pin_state && millis - p_button_state->time_last > p_config->button_debounce_ms)
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_config->toggle_press_time_ms)
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_config->toggle_press_time_ms)
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				p_button_state->current_state = 1;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt++;
			}
			// release button after push time
			else if (	millis - p_button_state->time_last > p_config->toggle_press_time_ms)
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if (p_config->buttons[*pos] == POV1_UP)
				{
					pov_buf[0] &= !(1 << 3);
					pov_buf[0] |= (p_button_state->pin_state << 3);
				}
				else if (p_config->buttons[*pos] == POV1_RIGHT)
				{
					pov_buf[0] &= !(1 << 2);
					pov_buf[0] |= (p_button_state->pin_state << 2);
				}
				else if (p_config->buttons[*pos] == POV1_DOWN)
				{
					pov_buf[0] &= !(1 << 1);
					pov_buf[0] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[0] &= !(1 << 0);
					pov_buf[0] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_config->button_debounce_ms )
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if (p_config->buttons[*pos] == POV2_UP)
				{
					pov_buf[1] &= !(1 << 3);
					pov_buf[1] |= (p_button_state->pin_state << 3);
				}
				else if (p_config->buttons[*pos] == POV2_RIGHT)
				{
					pov_buf[1] &= !(1 << 2);
					pov_buf[1] |= (p_button_state->pin_state << 2);
				}
				else if (p_config->buttons[*pos] == POV2_DOWN)
				{
					pov_buf[1] &= !(1 << 1);
					pov_buf[1] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[1] &= !(1 << 0);
					pov_buf[1] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_config->button_debounce_ms )
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if (p_config->buttons[*pos] == POV3_UP)
				{
					pov_buf[2] &= !(1 << 3);
					pov_buf[2] |= (p_button_state->pin_state << 3);
				}
				else if (p_config->buttons[*pos] == POV3_RIGHT)
				{
					pov_buf[2] &= !(1 << 2);
					pov_buf[2] |= (p_button_state->pin_state << 2);
				}
				else if (p_config->buttons[*pos] == POV3_DOWN)
				{
					pov_buf[2] &= !(1 << 1);
					pov_buf[2] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[2] &= !(1 << 0);
					pov_buf[2] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_config->button_debounce_ms )
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
								millis - p_button_state->time_last > p_config->button_debounce_ms)
			{
				p_button_state->changed = 0;
				//p_button_state->current_state = p_button_state->pin_state;
				p_button_state->prev_state = p_button_state->pin_state;
				p_button_state->cnt += p_button_state->pin_state;
				
				// set bit in povs data
				if (p_config->buttons[*pos] == POV4_UP)
				{
					pov_buf[3] &= !(1 << 3);
					pov_buf[3] |= (p_button_state->pin_state << 3);
				}
				else if (p_config->buttons[*pos] == POV4_RIGHT)
				{
					pov_buf[3] &= !(1 << 2);
					pov_buf[3] |= (p_button_state->pin_state << 2);
				}
				else if (p_config->buttons[*pos] == POV4_DOWN)
				{
					pov_buf[3] &= !(1 << 1);
					pov_buf[3] |= (p_button_state->pin_state << 1);
				}
				else
				{
					pov_buf[3] &= !(1 << 0);
					pov_buf[3] |= (p_button_state->pin_state << 0);
				}
			}
			// reset if state changed during debounce period
			else if (	p_button_state->changed && 
								millis - p_button_state->time_last > p_config->button_debounce_ms )
			{
				p_button_state->changed = 0;
			}
			break;
		
		default:
			break;
		
	}
}

/**
  * @brief  Checking single button state
  * @param  pin_num:	Number of pin where button is connected
	* @param  p_config: Pointer to device configuration
	* @param  pos: Pointer to button counter variable
  * @retval None
  */
void DirectButtonGet (uint8_t pin_num, app_config_t * p_config, uint8_t * pos)
{	
	if (p_config->pins[pin_num] == BUTTON_VCC)
	{
		raw_buttons_data[*pos] = GPIO_ReadInputDataBit(pin_config[pin_num].port, pin_config[pin_num].pin);
	}
	else 
	{
		raw_buttons_data[*pos] = !GPIO_ReadInputDataBit(pin_config[pin_num].port, pin_config[pin_num].pin);
	}	
}

/**
  * @brief  Checking all buttons routine
	* @param  p_config: Pointer to device configuration
  * @retval None
  */
void ButtonsCheck (app_config_t * p_config)
{
	uint8_t pos = 0;
	
	// get matrix buttons
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if ((p_config->pins[i] == BUTTON_COLUMN) && (pos < MAX_BUTTONS_NUM))
		{
			// tie Column pin to ground
			GPIO_WriteBit(pin_config[i].port, pin_config[i].pin, Bit_RESET);
			
			// get states at Rows
			for (int k=0; k<USED_PINS_NUM; k++)
			{
				if (p_config->pins[k] == BUTTON_ROW && pos < MAX_BUTTONS_NUM)
				{ 
					DirectButtonGet(k, p_config, &pos);
					pos++;
				}
			}
			// return Column pin to Hi-Z state
			GPIO_WriteBit(pin_config[i].port, pin_config[i].pin, Bit_SET);
		}
	}
	
	ShiftRegistersGet(raw_buttons_data, pov_pos, p_config, &pos);
	AxesToButtonsGet(raw_buttons_data, pov_pos, p_config, &pos);
	
	// check single buttons
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == BUTTON_GND || 
				p_config->pins[i] == BUTTON_VCC)
		{
			if (pos < MAX_BUTTONS_NUM)
			{
				DirectButtonGet(i, p_config, &pos);
				pos++;
			}
			else break;
		}
	}
	
	// convert encoders input
	EncoderProcess(buttons_state, p_config);
	
	// convert data to report format
	for (int i=0;i<pos;i++)
		{
			buttons_data[(i & 0xF8)>>3] &= ~(1 << (i & 0x07));
			buttons_data[(i & 0xF8)>>3] |= (buttons_state[i].current_state << (i & 0x07));
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
void ButtonsGet (uint8_t * raw_data, button_data_t * data)
{
	if (raw_data != NULL)
	{
		memcpy(raw_data, raw_buttons_data, sizeof(raw_buttons_data));
	}
	if (data != NULL)
	{
		memcpy(data, buttons_data, sizeof(buttons_data));
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



