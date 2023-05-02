#include "led_effects.h"
#include "cdc_data_handler.h"
#include "usb_endp.h"

static uint16_t AlgStep2, AlgStep3 = 0;//AlgStep, 
static uint8_t static_effect_set = 0;
static int32_t ticks = 0;

RGB_t const RainbowColors[] =
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
	static uint8_t first_start = 1;
	if (first_start) {
			// reset leds color
			if(p_dev_config->rgb_effect != WS2812B_STATIC) {
				ws2812b_SendRGB(p_dev_config->rgb_leds, p_dev_config->rgb_count);
			}
			first_start = 0;
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
			for (uint16_t i = NUM_RGB_LEDS; i > 1; i--)
			{
				rgb[i-1].r = rgb[i-2].r;
				rgb[i-1].g = rgb[i-2].g;
				rgb[i-1].b = rgb[i-2].b;
			}

			// time to smooth change color
			if (++AlgStep2 > RAINBOW_LENGTH)
			{
				AlgStep2 = 0;

				//loop colouers
				if (++AlgStep3 >= 7)
					AlgStep3 = 0;
			}

			// smooth color change of the zero pixel with a step of no more than RAINBOW_FADE_STEP
			StepChangeColor(&(rgb[0]), (RGB_t *) &(RainbowColors[AlgStep3]), RAINBOW_FADE_STEP);
			
			ws2812b_SendRGB(rgb, count);
		}
		break;
		
		default:
			break;
	}
}
