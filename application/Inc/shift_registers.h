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

#define SHIFTREG_TICK_DELAY			1

void ShiftRegistersInit(dev_config_t * p_dev_config);
void ShiftRegistersGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos);

#endif 	/* __SHIFT_REGISTERS_H__ */

