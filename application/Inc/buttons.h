/**
  ******************************************************************************
  * @file           : buttons.h
  * @brief          : Header for buttons.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "common_types.h"
#include "periphery.h"
#include "shift_registers.h"
#include "axis_to_buttons.h"

extern uint8_t									raw_buttons_data[MAX_BUTTONS_NUM];
extern logical_buttons_state_t 	logical_buttons_state[MAX_BUTTONS_NUM];
extern uint8_t									phy_buttons_data[MAX_BUTTONS_NUM/8];
extern uint8_t									log_buttons_data[MAX_BUTTONS_NUM/8];
extern uint8_t									shifts_state;

typedef uint8_t button_data_t;
typedef uint8_t pov_data_t;

void ButtonsDebounceProcess(dev_config_t * p_dev_config);
void LogicalButtonProcessState (logical_buttons_state_t * p_button_state, uint8_t * pov_buf, dev_config_t * p_dev_config, uint8_t pos);
void RadioButtons_Init (dev_config_t * p_dev_config);
void SequentialButtons_Init (dev_config_t * p_dev_config);
uint8_t ButtonsReadPhysical(dev_config_t * p_dev_config, uint8_t * p_buf);
void ButtonsReadLogical (dev_config_t * p_dev_config);
void ButtonsGet (uint8_t * out_data, uint8_t * log_data, uint8_t * phy_data, uint8_t * shift_data);
void POVsGet (pov_data_t * data);


#endif 	/* __BUTTONS_H__ */

