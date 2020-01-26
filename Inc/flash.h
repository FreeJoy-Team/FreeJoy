/**
  ******************************************************************************
  * @file           : flash.h
  * @brief          : Header for flash.c file.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H__
#define __FLASH_H__
	
	
#include "periphery.h"
#include "common_types.h"

void ConfigSet (app_config_t * p_config);
void ConfigGet (app_config_t * p_config);


#endif /* __FLASH_H__ */
