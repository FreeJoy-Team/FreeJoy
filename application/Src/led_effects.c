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

// TODO: library ws2812b uses a lot of memory
// in the future, it should be replaced with this one https://github.com/Crazy-Geeks/STM32-ARGB-DMA
// it needs to be adapted from HAL to SPL

static const uint8_t RAINBOW_FADE_STEP = 5;
static uint16_t AlgStep2, AlgStep3 = 0;
static uint8_t static_effect_set = 0;
static int32_t ticks = 0;
static uint8_t first_start = 1;

RGB_t RainbowColors[7] =
{
  {255,   0, 		0},        	// red
  {255, 165,		0},        	// orange
  {255, 255, 		0},        	// yellow
  {0, 	255,  	0},        	// green
  {0, 	255,  255},      		// cyan
  {0,   	0,  255},      		// blue
  {255, 	0,  255}       		// violet
};


void WS2812b_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length, int32_t current_ticks)
{
	if (first_start) {
		// reset leds color
		if(p_dev_config->rgb_effect != WS2812B_STATIC) {
			uint8_t leds_count = p_dev_config->rgb_count;
			RGB_t rgb[leds_count];
			for (uint8_t i = 0; i < leds_count; i ++)
			{
				rgb[i].r = 0;
				rgb[i].g = 0;
				rgb[i].b = 0;
			}
			ws2812b_SendRGB(rgb, leds_count);
			// load saved color, not reset. not sure about this, for some it will be a feature and for others it will be a bug
			// for reset comment this and uncomment ^^
			//ws2812b_SendRGB(p_dev_config->rgb_leds, leds_count);
		}
		
		if (p_dev_config->rgb_effect == WS2812B_RAINBOW) {
			for (uint8_t i = 0; i < 7; i++) {
				RainbowColors[i].r = (uint8_t)((uint16_t)(RainbowColors[i].r * p_dev_config->rgb_brightness) / 255);
				RainbowColors[i].g = (uint8_t)((uint16_t)(RainbowColors[i].g * p_dev_config->rgb_brightness) / 255);
				RainbowColors[i].b = (uint8_t)((uint16_t)(RainbowColors[i].b * p_dev_config->rgb_brightness) / 255);
			}
		}
		first_start = 0;
		return;
	}
	
	if (p_dev_config->rgb_effect == WS2812B_SIMHUB) 
	{
		SH_Process(p_dev_config, serial_num, sn_length);
	}
	else
	{
		if (ws2812b_IsReady() && ticks < current_ticks && !static_effect_set)
		{
			SetEffect(p_dev_config->rgb_leds, p_dev_config->rgb_count, p_dev_config->rgb_effect);
			ticks = current_ticks + p_dev_config->rgb_delay_ms;
		}
	}
}


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


void StepChangeColor(RGB_t *pLEDdesc, RGB_t *pLEDsource, uint8_t Step)
{
  StepChange(&(pLEDdesc->r), &(pLEDsource->r), Step);
  StepChange(&(pLEDdesc->g), &(pLEDsource->g), Step);
  StepChange(&(pLEDdesc->b), &(pLEDsource->b), Step);
}


void SetEffect(RGB_t *rgb, unsigned count, uint8_t effect)
{
	if (effect != WS2812B_STATIC) static_effect_set = 0;
	
	
	switch (effect)
	{
		case WS2812B_STATIC:
		{
			ws2812b_SendRGB(rgb, count);
			static_effect_set = 1;
		}
		break;
		
		case WS2812B_RAINBOW:
		{
			for (uint16_t i = count; i > 1; i--)
			{
				rgb[i-1] = rgb[i-2];
			}

			// time to smooth change color
			if (++AlgStep2 > count / 4)
			{
				AlgStep2 = 0;

				//loop colouers
				if (++AlgStep3 >= 7) AlgStep3 = 0;
			}

			// smooth color change of the zero pixel with a step of no more than RAINBOW_FADE_STEP
			StepChangeColor(rgb, (RGB_t *) &(RainbowColors[AlgStep3]), RAINBOW_FADE_STEP);
			
			ws2812b_SendRGB(rgb, count);
		}
		break;
		
		case WS2812B_FLOW:
		{
			//if (count >= 2) {
				RGB_t temp = rgb[count - 1];
				for (int16_t i = count - 2; i >= 0; i--)
				{
					rgb[i+1] = rgb[i];
				}
				rgb[0] = temp;
			//}
			
			ws2812b_SendRGB(rgb, count);
		}
		break;
		
		default:
			break;
	}
}
