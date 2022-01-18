/**
  ******************************************************************************
  * @file           : config.h
  * @brief          : Header for config.c file.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H__
#define __CONFIG_H__
	
	
#include "periphery.h"
#include "common_types.h"

void DevConfigSet (dev_config_t * p_dev_config);
void DevConfigGet (dev_config_t * p_dev_config);
void AppConfigInit (dev_config_t * p_dev_config);
void AppConfigGet (app_config_t * p_app_config);
uint8_t IsAppConfigEmpty (app_config_t * p_app_config);

#endif /* __CONFIG_H__ */
