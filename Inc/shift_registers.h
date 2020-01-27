/**
  ******************************************************************************
  * @file           : shift_registers.h
  * @brief          : Header for shift_registers.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SHIFT_REGISTERS_H__
#define __SHIFT_REGISTERS_H__

#include "common_types.h"
#include "periphery.h"

void ShiftRegistersInit(app_config_t * p_config);
void ShiftRegistersGet (uint8_t * raw_button_data_buf, uint8_t * pov_buf, app_config_t * p_config, uint8_t * pos);

#endif 	/* __SHIFT_REGISTERS_H__ */

