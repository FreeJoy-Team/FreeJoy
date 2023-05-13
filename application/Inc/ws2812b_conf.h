// The MIT License (MIT)
//
// Copyright (c) 2015 Aleksandr Aleshin <silencer@quadrius.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __WS2812B_CONF_H
#define __WS2812B_CONF_H

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dma.h>
#include <misc.h>

#define WS2812B_USE_GAMMA_CORRECTION
#define WS2812B_USE_PRECALCULATED_GAMMA_TABLE

#define WS2812B_BUFFER_SIZE     60
#define WS2812B_START_SIZE      2

// TODO: add "is advanced timer"

//#define WS2812B_APB1_RCC        RCC_APB1Periph_TIM3
//#define WS2812B_APB2_RCC        RCC_APB2Periph_GPIOB

//#define WS2812B_AHB_RCC         RCC_AHBPeriph_DMA1

//#define WS2812B_GPIO            GPIOB
//#define WS2812B_GPIO_PIN        GPIO_Pin_4

//#define WS2812B_TIM             TIM3
//#define WS2812B_TIM_OCINIT      TIM_OC1Init
//#define WS2812B_TIM_OCPRELOAD   TIM_OC1PreloadConfig
//#define WS2812B_TIM_DMA_CC      TIM_DMA_CC1
//#define WS2812B_TIM_DMA_CCR     (WS2812B_TIM->CCR1)
//#define WS2812B_TIM_PWM_MODE		TIM_OCMode_PWM1

//#define WS2812B_GPIO_Mode				GPIO_Mode_AF_PP




#define WS2812B_APB1_RCC        NULL
#define WS2812B_APB2_RCC        RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA // see void ws2812b_Init(void)

#define WS2812B_AHB_RCC         RCC_AHBPeriph_DMA1

#define WS2812B_GPIO            GPIOA
#define WS2812B_GPIO_PIN        GPIO_Pin_10

#define WS2812B_TIM             TIM1
#define WS2812B_TIM_OCINIT      TIM_OC3Init
#define WS2812B_TIM_OCPRELOAD   TIM_OC3PreloadConfig
#define WS2812B_TIM_DMA_CC      TIM_DMA_CC3
#define WS2812B_TIM_DMA_CCR     (WS2812B_TIM->CCR3)
#define WS2812B_TIM_PWM_MODE		TIM_OCMode_PWM2

#define WS2812B_GPIO_Mode				GPIO_Mode_AF_OD




#define WS2812B_DMA             DMA1
#define WS2812B_DMA_CHANNEL     DMA1_Channel6
#define WS2812B_DMA_IRQ         DMA1_Channel6_IRQn

#define WS2812B_DMA_HANDLER     DMA1_Channel6_IRQHandler
#define WS2812B_DMA_IT_TC       DMA1_IT_TC6
#define WS2812B_DMA_IT_HT       DMA1_IT_HT6

#define WS2812B_IRQ_PRIO        2
#define WS2812B_IRQ_SUBPRIO     2

#define WS2812B_FREQUENCY       72000000
#define WS2812B_PERIOD          90//91?

#define WS2812B_PULSE_HIGH      30//65
#define WS2812B_PULSE_LOW       60//26

#endif //__WS2812B_CONF_H
