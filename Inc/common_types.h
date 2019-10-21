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


typedef struct
{
	int8_t point1;
	int8_t point2;
	int8_t point3;
	int8_t point4;
	int8_t point5;
	int8_t point6;
	int8_t point7;
	int8_t point8;
	int8_t point9;
	int8_t point10;
	
}curve_shape_t;

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
	curve_shape_t 	curve_shape;
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
	
	ENCODER_SINGLE_INPUT,
	ENCODER_CHAINED_INPUT,
	ENCODER_CHAINED_COMMON,
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
	
	BUTTON_TO_ANALOG,
	BUTTON_SHIFT,
	
};
typedef uint8_t button_t;


enum
{
	ENCODER_1_1 = 0,
	ENCODER_1_2,
	ENCODER_1_4,
};	
typedef uint8_t encoder_type_t;

typedef struct
{
	uint8_t 				pin_a;
	uint8_t 				pin_b;
	uint8_t 				pin_c;
	encoder_type_t 	type;
	
} encoder_t;

typedef struct 
{
	// config 1
	uint16_t 				firmware_version;
	char 						device_name[10];
	uint16_t				button_debounce_ms;
	uint16_t				toggle_press_time_ms;
	uint16_t				encoder_press_time_ms;
	uint16_t 				exchange_period_ms;	
	pin_t 					pins[USED_PINS_NUM];
	
	// config 2-5
	axis_config_t 	axis_config[MAX_AXIS_NUM];

	// config 6-7
	button_t 				buttons[MAX_BUTTONS_NUM];

	// config 8
	encoder_t 			encoders[MAX_ENCODERS_NUM];
	
	// config 9-10
	uint8_t					reserved_1[62];
	uint8_t					reserved_2[62];
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
