#ifndef _LED_EFFECTS_H
#define _LED_EFFECTS_H

#include "common_types.h"
#include "ws2812b.h"

//#define SequentalLights_LedNum          20
//#define SequentalLights_Period          20
//#define SequentalLights_CycleNum        15

#define RAINBOW_LENGTH             40 // change
#define RAINBOW_FADE_STEP          8

void WS2812b_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length, int32_t current_ticks);
void SetEffect(RGB_t *rgb, unsigned count, uint8_t effect);

#endif
