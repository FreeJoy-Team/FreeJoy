/**
  ******************************************************************************
  * @file           : encoders.c
  * @brief          : Encoders driver implementation
  ******************************************************************************
  */

#include "encoders.h"
//#include "string.h"


encoder_t encoders_state[MAX_ENCODERS_NUM];

void EncoderProcess (buttons_state_t * button_state_buf, app_config_t * p_config)
{	
	for (int i=0; i<MAX_ENCODERS_NUM; i++)
	{
		uint32_t millis = HAL_GetTick();
		
		if (encoders_state[i].pin_a >= 0 && 
				encoders_state[i].pin_b >= 0 && 
				button_state_buf[encoders_state[i].pin_b].pin_state &&
				!button_state_buf[encoders_state[i].pin_b].pin_prev_state &&
				button_state_buf[encoders_state[i].pin_a].pin_state)							// CW
		{
			encoders_state[i].state_cw = 1;
			encoders_state[i].cnt++;
			encoders_state[i].time_last = millis;
				
			button_state_buf[encoders_state[i].pin_a].current_state = 1;		
		}
		if (encoders_state[i].pin_a >= 0 && 
				encoders_state[i].pin_b >= 0 && 
				button_state_buf[encoders_state[i].pin_a].pin_state &&
				!button_state_buf[encoders_state[i].pin_a].pin_prev_state &&
				button_state_buf[encoders_state[i].pin_b].pin_state)							// CCW
		{
			encoders_state[i].state_ccw = 1;
			encoders_state[i].cnt--;
			encoders_state[i].time_last = millis;
				
			button_state_buf[encoders_state[i].pin_b].current_state = 1;			
		}
		if (millis - encoders_state[i].time_last > p_config->encoder_press_time_ms)
		{
			encoders_state[i].state_ccw = 0;
			encoders_state[i].state_cw = 0;
			
			button_state_buf[encoders_state[i].pin_a].current_state = 0;
			button_state_buf[encoders_state[i].pin_b].current_state = 0;
		}
	}
}

void EncodersInit(app_config_t * p_config)
{
	uint8_t pos = 0;
	int8_t prev_a = -1;
	int8_t prev_b = -1;
	
	for (int i=0; i<MAX_ENCODERS_NUM; i++)
	{
		encoders_state[i].pin_a = -1;
		encoders_state[i].pin_b = -1;
	}
	
	for (int i=0; i<MAX_BUTTONS_NUM; i++)
	{
		if (p_config->buttons[i] == ENCODER_INPUT_A && i > prev_a)
		{
			for (int j=0; j<MAX_BUTTONS_NUM; j++)
			{
				if (p_config->buttons[j] == ENCODER_INPUT_B && j > prev_b)
				{
					encoders_state[pos].pin_a = i;
					encoders_state[pos].pin_b = j;
					
					prev_a = i;
					prev_b = j;
					pos++;
					break;
				}
			}
		}
		
	}
}
