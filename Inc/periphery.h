/**
  ******************************************************************************
  * @file           : periphery.h
  * @brief          : Header for periphery.c file.
  *                   This file contains the common defines of the periphery.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PERIPHERY_H__
#define __PERIPHERY_H__

#include "stm32f1xx_hal.h"
#include "common_types.h"


typedef struct 
{
	GPIO_TypeDef * 	port;
	uint16_t				pin;
	uint8_t 				number;
	
}	pin_config_t;


void GetPinConfig (pin_config_t ** p_config);
void SystemClock_Config(void);
void GPIO_Init (app_config_t * p_config);





#endif 	/* __PERIPHERY_H__ */
