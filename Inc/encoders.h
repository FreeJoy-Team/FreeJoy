/**
  ******************************************************************************
  * @file           : encoders.h
  * @brief          : Header for encoders.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODERS_H__
#define __ENCODERS_H__

#include "common_types.h"
#include "periphery.h"


void EncoderProcess (buttons_state_t * button_state_buf, app_config_t * p_config);
void EncodersInit (app_config_t * p_config);

#endif 	/* __BUTTONS_H__ */

