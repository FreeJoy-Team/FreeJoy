/**
  ******************************************************************************
  * @file           : encoders.c
  * @brief          : Encoders driver implementation
  ******************************************************************************
  */

#include "encoders.h"

uint8_t		physical_buttons_state[MAX_BUTTONS_NUM];

#if (ENC_COUNT == 1)
int8_t enc_array [16] =
{
0,  0,  0,  0,
-1,  0,  0,  0,
1,  0,  0,  0,
0,  0,  0,  0
};
#elif (ENC_COUNT == 2)
int8_t enc_array [16] =
{
0,  0,  0,  0,
-1,  0,  0,  1,
1,  0,  0, -1,
0,  0,  0,  0
};
#else
int8_t enc_array [16] =
{
0,  1, -1,  0,
-1,  0,  0,  1,
1,  0,  0, -1,
0, -1,  1,  0
};
#endif

encoder_t encoders_state[MAX_ENCODERS_NUM];

void EncoderProcess (buttons_state_t * button_state_buf, app_config_t * p_config)
{	
	
	ButtonsReadPhysical(p_config, physical_buttons_state);
	
	
	for (int i=0; i<MAX_ENCODERS_NUM; i++)
	{
		uint32_t millis = GetTick();
		
		{
			int8_t stt;
			encoders_state[i].state <<= 2;			// shift prev state to clear space for new data
			
			if (physical_buttons_state[p_config->buttons[encoders_state[i].pin_a].physical_num])	encoders_state[i].state |= 0x01;		// Pin A high
			if (physical_buttons_state[p_config->buttons[encoders_state[i].pin_b].physical_num])	encoders_state[i].state |= 0x02;		// Pin B high
			
			if ((encoders_state[i].state & 0x03) != ((encoders_state[i].state >> 2) & 0x03))							// Current state != Prev state
			{
				stt = enc_array[encoders_state[i].state & 0x0F];
				
				if (stt != 0)		// changed
				{
					if ((encoders_state[i].state & 0x03) != ((encoders_state[i].state >> 4) & 0x03))   // if encoder didnt come back
					{
						encoders_state[i].dir = stt > 0 ? 1 : -1;
						if (millis - encoders_state[i].time_last > 50 || encoders_state[i].dir == encoders_state[i].last_dir)	// if direction didnt change too fast
						{
							if (stt > 0)	
							{
								button_state_buf[encoders_state[i].pin_a].current_state = 1;			// CW
								encoders_state[i].last_dir = 1;
							}
							else
							{
								button_state_buf[encoders_state[i].pin_b].current_state = 1;			// CCW
								encoders_state[i].last_dir = -1;
							}						
							encoders_state[i].time_last = millis;
							encoders_state[i].cnt += stt;
						}
						else if (millis - encoders_state[i].time_last <= 200 && encoders_state[i].dir != encoders_state[i].last_dir)
						{
							encoders_state[i].time_last = millis;
							encoders_state[i].cnt += encoders_state[i].last_dir;
							if (encoders_state[i].last_dir > 0)	button_state_buf[encoders_state[i].pin_a].current_state = 1;
							else button_state_buf[encoders_state[i].pin_b].current_state = 1;
						}
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
		if ((p_config->buttons[i].type) == ENCODER_INPUT_A &&  i > prev_a)
		{
			for (int j=0; j<MAX_BUTTONS_NUM; j++)
			{
				if ((p_config->buttons[j].type) == ENCODER_INPUT_B && j > prev_b && pos < MAX_ENCODERS_NUM)
				{
					encoders_state[pos].pin_a = i;
					encoders_state[pos].pin_b = j;
					encoders_state[pos].dir = 1;
					encoders_state[pos].last_dir = 1;
					
					prev_a = i;
					prev_b = j;
					pos++;
					break;
				}
			}
		}	
	}
}
