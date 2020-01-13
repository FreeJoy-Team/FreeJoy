/**
  ******************************************************************************
  * @file           : common_types.h
  * @brief          : This file contains the common types for the app.                  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "stdint.h"
#include "common_defines.h"

enum
{
	FILTER_NO = 0,
	FILTER_LOW,
	FILTER_MEDIUM,
	FILTER_HIGH,
	
}; 
typedef uint8_t filter_t;

typedef struct
{
	uint16_t 				calib_min;
	uint16_t				calib_center;
	uint16_t 				calib_max;
	uint8_t 				autocalib;
	uint8_t					inverted;
	filter_t 				filter;
	int8_t				 	curve_shape[10];
	uint8_t					reserved[10];
} axis_config_t;

enum
{
	NOT_USED = 0,
	
	BUTTON_GND,
	BUTTON_VCC,
	BUTTON_ROW,
	BUTTON_COLUMN,
	
	AXIS_ANALOG,
	AXIS_TO_BUTTONS,
	
	SPI_SCK,

  TLE5011_CS,
  TLE5011_DATA,
  TLE5011_GEN,

  SHIFT_REG_CS,
  SHIFT_REG_DATA,

};
typedef uint8_t pin_t;

enum
{
	BUTTON_NORMAL = 0,
	BUTTON_INVERTED,
	BUTTON_TOGGLE,
	TOGGLE_SWITCH,
	TOGGLE_SWITCH_ON,
	TOGGLE_SWITCH_OFF,
	
	POV1_UP,
	POV1_RIGHT,
	POV1_DOWN,
	POV1_LEFT,
	POV2_UP,
	POV2_RIGHT,
	POV2_DOWN,
	POV2_LEFT,
	POV3_UP,
	POV3_RIGHT,
	POV3_DOWN,
	POV3_LEFT,
	POV4_UP,
	POV4_RIGHT,
	POV4_DOWN,
	POV4_LEFT,
	
	ENCODER_INPUT_A,
	ENCODER_INPUT_B,
	
};
typedef uint8_t button_t;

typedef struct buttons_state_t
{
	uint8_t pin_state;
	uint8_t pin_prev_state;
	uint8_t prev_state;
	uint8_t current_state;
	uint8_t changed;
	uint64_t time_last;	
	uint8_t cnt;
	
} buttons_state_t;


typedef struct
{
	uint8_t 				state_cw;
	uint8_t 				state_ccw;
	uint64_t 				time_last;	
	int16_t 				cnt;	
	int8_t 					pin_a;
	int8_t 					pin_b;
	
} encoder_t;

typedef struct
{
	int8_t points[12];
	uint8_t buttons_cnt;
	uint8_t is_analog_enabled;
	
} axis_to_buttons_t;

enum
{
	HC165 = 0,
	CD4021 = 1,
};	
typedef uint8_t shift_register_type_t;

typedef struct
{	
	uint8_t 				type;
	int8_t 					button_cnt;	
	uint8_t 				reserved[2];
	
	
} shift_register_t;

typedef struct 
{
	// config 1
	uint16_t 						firmware_version;
	char 								device_name[20];
	uint16_t						button_debounce_ms;
	uint16_t						toggle_press_time_ms;
	uint16_t						encoder_press_time_ms;
	uint16_t 						exchange_period_ms;	
	uint8_t							reserved_1[2];
	pin_t 							pins[USED_PINS_NUM];
	
	
	// config 2-5
	axis_config_t 			axis_config[MAX_AXIS_NUM];
	uint8_t							reserved_5[8];
	// config 6-7-8
	button_t 						buttons[MAX_BUTTONS_NUM];
	
	// config 8-9
	axis_to_buttons_t		axes_to_buttons[MAX_AXIS_NUM];
	uint8_t							reserved_9[8];
	
	// config 10	
	shift_register_t		shift_registers[4];
	uint8_t							reserved_10[46];
}app_config_t;

typedef struct
{
	uint8_t 		dummy;
	uint8_t 		id;
	uint8_t 		button_data[MAX_BUTTONS_NUM/8];
	uint16_t 		axis_data[MAX_AXIS_NUM];
	uint8_t 		pov_data[MAX_POVS_NUM];
	uint16_t		raw_axis_data[MAX_AXIS_NUM];
	
} joy_report_t;






#endif 	/* __COMMON_TYPES_H__ */
