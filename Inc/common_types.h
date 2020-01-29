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


#define SHIFT1_MASK					0x20
#define SHIFT2_MASK					0x40
#define SHIFT3_MASK					0x60
#define SHIFT4_MASK					0x80
#define SHIFT5_MASK					0xA0

#define BUTTON_TYPE_MASK		0x1F

enum
{
	FILTER_NO = 0,
	FILTER_LOW,
	FILTER_MEDIUM,
	FILTER_HIGH,
	
}; 
typedef uint8_t filter_t;

typedef int16_t analog_data_t;

typedef struct
{
	analog_data_t 	calib_min;
	analog_data_t		calib_center;
	analog_data_t 	calib_max;
	uint8_t 				magnet_offset;
	uint8_t					inverted;
	filter_t 				filter;
	int8_t				 	curve_shape[11];
	uint8_t 				out_enabled;
	uint8_t					resolution;
	uint8_t					reserved[7];
} axis_config_t;

enum
{
	NOT_USED = 0,
	
	BUTTON_GND,
	BUTTON_VCC,
	BUTTON_ROW,
	BUTTON_COLUMN,
	
	AXIS_ANALOG,
	
//	RESERVED,
	
	SPI_SCK = 7,

  TLE5011_CS,
  SPI_DATA,
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
typedef uint8_t button_type_t;

typedef struct button_t
{
	int8_t					physical_num;
	button_type_t 	type;
	
}	button_t;

typedef struct buttons_state_t
{
  uint64_t time_last;	
	uint8_t pin_state;
	//uint8_t pin_prev_state;
	uint8_t prev_state;
	uint8_t current_state;
	uint8_t changed;	
	uint8_t cnt;
	
} buttons_state_t;


typedef struct
{
  uint64_t 				time_last;
	uint8_t 				state;
	int16_t 				cnt;	
	int8_t 					pin_a;
	int8_t 					pin_b;
	
} encoder_t;

typedef struct
{
	int8_t points[13];
	uint8_t buttons_cnt;
	uint8_t is_enabled;
	
} axis_to_buttons_t;

enum
{
	HC165 = 0,
	CD4021 = 1,
};	
typedef uint8_t shift_reg_config_type_t;

typedef struct
{	
	uint8_t 			type;
	uint8_t 			button_cnt;	
	int8_t 				pin_cs;
	int8_t 				pin_data;
	
} shift_reg_config_t;

enum
{
	SHIFT_NORMAL = 0,
	SHIFT_INVERTED,
	
};

typedef struct 
{
	int8_t 				button;
	
} shift_modificator_t;

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
	
	// config 6-7-8-9-10
	button_t 						buttons[MAX_BUTTONS_NUM];
	
	// config 10-11-12
	axis_to_buttons_t		axes_to_buttons[MAX_AXIS_NUM];
	
	// config 12	
	shift_reg_config_t	shift_registers[4];
	shift_modificator_t	shift_config[5];
	uint8_t							reserved_10[31];
}app_config_t;

typedef struct
{
	uint8_t 				dummy;
	uint8_t 				id;
	uint8_t 				button_data[MAX_BUTTONS_NUM/8];
	int16_t			 		axis_data[MAX_AXIS_NUM];
	uint8_t 				pov_data[MAX_POVS_NUM];
	int16_t					raw_axis_data[MAX_AXIS_NUM];
	uint8_t					raw_button_data[9];
	
} joy_report_t;




#endif 	/* __COMMON_TYPES_H__ */
