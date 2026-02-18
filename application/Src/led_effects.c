/**
  ******************************************************************************
  * @file           : led_effects.c
  * @brief          : led effects processing implementation
		
		FreeJoy software for game device controllers
    Copyright (C) 2020  Yury Vostrenkov (yuvostrenkov@gmail.com)
		and Reksotiv (https://github.com/Reksotiv)

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

#include "led_effects.h"
#include "ws2812b.h"
#include "simhub.h"
#include "usb_endp.h"
#include "buttons.h"

// TODO: library ws2812b uses a lot of memory
// in the future, it should be replaced with this one https://github.com/Crazy-Geeks/STM32-ARGB-DMA
// it needs to be adapted from HAL to SPL

void LedEffect_Init(dev_config_t * p_dev_config);
void SetEffect(argb_led_t *leds, unsigned count, uint8_t effect);

// static const uint8_t RAINBOW_FADE_STEP = 5;
#define FADE_STEP				 24//ceil(RAINBOW_FADE_STEP * 255.0f / m_brightness)

static uint8_t current_fade_step = FADE_STEP;
static uint16_t AlgStep2, AlgStep3 = 0;
static uint8_t static_effect_set = 0;
static int32_t ticks = 0;
static uint8_t first_start = 1;
static uint8_t button_detected = 0;

static const rgb_t RB_COLORS[7] =
{
  {255,   0, 		0},        	// red
  {255, 165,		0},        	// orange
  {255, 255, 		0},        	// yellow
  {0, 	255,  	0},        	// green
  {0, 	255,  255},      		// cyan
  {0,   	0,  255},      		// blue
  {255, 	0,  255}       		// violet
};

static rgb_t RainbowColors[7] =
{
  {255,   0, 		0},        	// red
  {255, 165,		0},        	// orange
  {255, 255, 		0},        	// yellow
  {0, 	255,  	0},        	// green
  {0, 	255,  255},      		// cyan
  {0,   	0,  255},      		// blue
  {255, 	0,  255}       		// violet
};


void LedEffect_Init(dev_config_t * p_dev_config)
{
	// brightness
		if (p_dev_config->rgb_effect == WS2812B_RAINBOW) {
			for (uint8_t i = 0; i < NUM_RGB_LEDS; i ++)
			{
				p_dev_config->rgb_leds[i].color.r = 0;
				p_dev_config->rgb_leds[i].color.g = 0;
				p_dev_config->rgb_leds[i].color.b = 0;
			}
			
			for (uint8_t i = 0; i < 7; i++) {
				RainbowColors[i].r = (uint8_t)((uint16_t)(RB_COLORS[i].r * p_dev_config->rgb_brightness) / 255);
				RainbowColors[i].g = (uint8_t)((uint16_t)(RB_COLORS[i].g * p_dev_config->rgb_brightness) / 255);
				RainbowColors[i].b = (uint8_t)((uint16_t)(RB_COLORS[i].b * p_dev_config->rgb_brightness) / 255);
			}
			current_fade_step = FADE_STEP * p_dev_config->rgb_brightness / 255 + 1;
			ws2812b_SendRGB(p_dev_config->rgb_leds, NUM_RGB_LEDS);
		}
		else if (p_dev_config->rgb_effect == WS2812B_STATIC || p_dev_config->rgb_effect == WS2812B_FLOW)
		{
			// reset leds color
			argb_led_t rgb[NUM_RGB_LEDS];
			for (uint8_t i = 0; i < NUM_RGB_LEDS; i ++)
			{
				rgb[i].color.r = 0;
				rgb[i].color.g = 0;
				rgb[i].color.b = 0;
			}
			
			for (uint8_t i = 0; i < p_dev_config->rgb_count; i++) 
			{
				p_dev_config->rgb_leds[i].color.r = (uint8_t)((uint16_t)(p_dev_config->rgb_leds[i].color.r * p_dev_config->rgb_brightness) / 255);
				p_dev_config->rgb_leds[i].color.g = (uint8_t)((uint16_t)(p_dev_config->rgb_leds[i].color.g * p_dev_config->rgb_brightness) / 255);
				p_dev_config->rgb_leds[i].color.b = (uint8_t)((uint16_t)(p_dev_config->rgb_leds[i].color.b * p_dev_config->rgb_brightness) / 255);
			}
			ws2812b_SendRGB(rgb, NUM_RGB_LEDS);
		}
		else
		{
			// reset leds color
			argb_led_t rgb[NUM_RGB_LEDS];
			for (uint8_t i = 0; i < NUM_RGB_LEDS; i ++)
			{
				rgb[i].color.r = 0;
				rgb[i].color.g = 0;
				rgb[i].color.b = 0;
			}
			ws2812b_SendRGB(rgb, NUM_RGB_LEDS);
		}
		
		for (uint8_t i = 0; i < p_dev_config->rgb_count; i++) 
		{
			if (p_dev_config->rgb_leds[i].input_num >= 0) 
			{
				button_detected = 1;
				break;
			}
		}
		
		first_start = 0;
}



void ArgbLed_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length, int32_t current_ticks)
{
	if (!ws2812b_IsReady()) return;
	
	if (first_start) {
		LedEffect_Init(p_dev_config);
		return;
	}
	
	uint8_t need_update = 0;
	
	if (p_dev_config->rgb_effect == WS2812B_SIMHUB) 
	{
		need_update = SH_Process(p_dev_config, serial_num, sn_length);
	}
	else
	{
		if (ticks < current_ticks && !static_effect_set)
		{
			SetEffect(p_dev_config->rgb_leds, p_dev_config->rgb_count, p_dev_config->rgb_effect);
			ticks = current_ticks + p_dev_config->rgb_delay_ms;
			need_update = 1;
		}
	}
	
	
	// buttons
	if (button_detected)
	{
		int8_t input_num = -1;
	
		for (uint8_t i = 0; i < p_dev_config->rgb_count; i++)
		{
			input_num = p_dev_config->rgb_leds[i].input_num;
			if (input_num >= 0)
			{
				uint8_t but_state = logical_buttons_state[input_num].current_state;
				
				if (p_dev_config->buttons[input_num].is_inverted)
				{
					but_state = !but_state;
				}
				
				if (p_dev_config->rgb_leds[i].is_inverted)
				{
					but_state = !but_state;
				}
				
				if (p_dev_config->rgb_leds[i].is_disabled != but_state)
				{
					need_update = 1;
				}
				
				if (!but_state)
				{
					p_dev_config->rgb_leds[i].is_disabled = 1;
				}
				else
				{
					p_dev_config->rgb_leds[i].is_disabled = 0;
				}
			}
		}
		//need_update = 1;
	}
	
	
	// update led
	if (need_update)
	{
		ws2812b_SendRGB(p_dev_config->rgb_leds, p_dev_config->rgb_count);
	}
}

//void UpdateLEDs(void)
//{
//	static_effect_set = 0;
//	first_start = 1;
//}

void StepChange(uint8_t *desc, uint8_t *source, uint8_t Step)
{
  if (*desc < *source)
  {
    if ((0xFF - *desc) < Step)
      *desc = 0xFF;
    else
      *desc += Step;
  }
    
  if (*desc > *source)
  {
    if (*desc > Step)
      *desc -= Step;
    else
      *desc = 0;
  }
}       


void StepChangeColor(rgb_t *led_desc, rgb_t *led_source, uint8_t Step)
{
  StepChange(&(led_desc->r), &(led_source->r), Step);
  StepChange(&(led_desc->g), &(led_source->g), Step);
  StepChange(&(led_desc->b), &(led_source->b), Step);
}


void SetEffect(argb_led_t *leds, unsigned count, uint8_t effect)
{
	if (effect != WS2812B_STATIC) static_effect_set = 0;
	
	
	switch (effect)
	{
		case WS2812B_STATIC:
		{
			static_effect_set = 1;
		}
		break;
		
		case WS2812B_RAINBOW:
		{
			for (uint8_t i = count; i > 1; i--)
			{
				leds[i-1].color = leds[i-2].color;
			}

			// time to smooth change color
			if (++AlgStep2 > count / 4)
			{
				AlgStep2 = 0;

				//loop colouers
				if (++AlgStep3 >= 7) AlgStep3 = 0;
			}

			// smooth color change of the zero pixel with a step of no more than RAINBOW_FADE_STEP
			StepChangeColor(&leds[0].color, (rgb_t *) &(RainbowColors[AlgStep3]), current_fade_step);
		}
		break;
		
		case WS2812B_FLOW:
		{
			//if (count >= 2) {
				rgb_t temp = leds[count - 1].color;
				for (int16_t i = count - 2; i >= 0; i--)
				{
					leds[i+1].color = leds[i].color;
				}
				leds[0].color = temp;
			//}
		}
		break;
		
		default:
			break;
	}
}
