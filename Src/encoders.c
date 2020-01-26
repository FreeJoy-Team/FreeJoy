/**
  ******************************************************************************
  * @file           : encoders.c
  * @brief          : Encoders driver implementation
  ******************************************************************************
  */

#include "encoders.h"

int8_t enc_array [16] =
{
0,  0,  0,  0,
-1,  0,  0,  0,
1,  0,  0,  0,
0,  0,  0,  0
};


encoder_t encoders_state[MAX_ENCODERS_NUM];

void EncoderProcess (buttons_state_t * button_state_buf, app_config_t * p_config)
{	
	for (int i=0; i<MAX_ENCODERS_NUM; i++)
	{
		uint32_t millis = GetTick();
		
		{
			int8_t stt;
			encoders_state[i].state <<= 2;			// shift prev state to clear space for new data
			
			if (button_state_buf[encoders_state[i].pin_a].pin_state)	encoders_state[i].state |= 0x01;		// Pin A high
			if (button_state_buf[encoders_state[i].pin_b].pin_state)	encoders_state[i].state |= 0x02;		// Pin B high
			
			if ((encoders_state[i].state & 0x03) != ((encoders_state[i].state >> 2) & 0x03))							// Current state != Prev state
			{
				stt = enc_array[encoders_state[i].state & 0x0F];
				
				if (stt != 0)		// changed
				{
					if ((encoders_state[i].state & 0x03) != ((encoders_state[i].state >> 4) & 0x03))   // if encoder didnt come back
					{
						if (stt > 0)	button_state_buf[encoders_state[i].pin_a].current_state = 1;			// CW
						else 					button_state_buf[encoders_state[i].pin_b].current_state = 1;			// CCW
						
						encoders_state[i].time_last = millis;
						encoders_state[i].cnt += stt;
					}
				}
			}
			else	
			{
				encoders_state[i].state >>= 2;
			}
		}
		
		if (millis - encoders_state[i].time_last > p_config->encoder_press_time_ms)
		{	
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
		if (p_config->buttons[i] == ENCODER_INPUT_A &&  i > prev_a)
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
