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


/******************** AXIS **********************/
enum
{
	FILTER_NO = 0,
	FILTER_LEVEL_1,
	FILTER_LEVEL_2,
	FILTER_LEVEL_3,
	FILTER_LEVEL_4,
	FILTER_LEVEL_5,
	FILTER_LEVEL_6,
	FILTER_LEVEL_7,
}; 
typedef uint8_t filter_t;

typedef int16_t analog_data_t;

enum
{
	NO_FUNCTION = 0,
	FUNCTION_PLUS,
	FUNCTION_MINUS,
	FUNCTION_EQUAL,
};

enum
{
	AXIS_BUTTON_FUNC_EN = 0,
	AXIS_BUTTON_PRESCALER_EN,
	AXIS_BUTTON_CENTER,
	AXIS_BUTTON_RESET,
	AXIS_BUTTON_DOWN,
	AXIS_BUTTON_UP,
};

typedef struct
{
	analog_data_t 	calib_min;
	analog_data_t		calib_center;
	analog_data_t 	calib_max;
	uint8_t					out_enabled: 	1;
	uint8_t					inverted: 		1;
	uint8_t					is_centered: 	1;
	uint8_t					function: 		2;
	uint8_t 				filter: 			3;
	
	int8_t				 	curve_shape[11];
	uint8_t					resolution : 4;
	uint8_t					channel : 4;
	uint8_t					deadband_size: 7;
	uint8_t					is_dynamic_deadband: 1;
	
	int8_t					source_main;
	uint8_t					source_secondary: 3;
	uint8_t					offset_angle: 5;
	
	int8_t					button1;
	int8_t					button2;
	int8_t					button3;
	uint8_t					divider;
	uint8_t					i2c_address;
	uint8_t					button1_type	:3;
	uint8_t					button2_type	:2;
	uint8_t					button3_type	:3;
	uint8_t					prescaler;
	uint8_t					reserved[1];
	
} axis_config_t;

enum
{
	SOURCE_ENCODER = -3,
	SOURCE_I2C = -2,
	SOURCE_NO = -1,
};
typedef int8_t axis_source_t;

enum
{
	ANALOG = 0,
	TLE5011,
	MCP3201,
	MCP3202,
	MCP3204,
	MCP3208,
	MLX90393_SPI,
	MLX90393_I2C,
	ADS1115,
	AS5600,
	AS5048A_SPI,
	TLE5012,
	MLX90363,
	
};

typedef struct
{
	uint32_t	ok_cnt;
	uint32_t 	err_cnt;
	
	uint8_t 	rx_complete;
	uint8_t 	tx_complete;
	uint8_t		curr_channel;	
	
	int8_t 		source;
	uint8_t		type;
	uint8_t		address;
	
	uint8_t 	data[24];
	
	
} sensor_t;


/******************** PINS **********************/
enum
{
	NOT_USED = 0,
	
	BUTTON_GND,
	BUTTON_VCC,
	BUTTON_ROW,
	BUTTON_COLUMN,
	
	AXIS_ANALOG,
	FAST_ENCODER,
	
	SPI_SCK,
	SPI_MOSI,
	SPI_MISO,

	TLE5011_GEN,
	TLE5011_CS,
	TLE5012_CS,
  

	MCP3201_CS,
	MCP3202_CS,
	MCP3204_CS,
	MCP3208_CS,

	MLX90393_CS,

	AS5048A_CS,
	
	SHIFT_REG_LATCH,
	SHIFT_REG_DATA,
	
	LED_PWM,
	LED_SINGLE,
	LED_ROW,
	LED_COLUMN,
	
	I2C_SCL,
	I2C_SDA,
	
	MLX90363_CS,
	SHIFT_REG_CLK,
	
	LED_RGB,
};
typedef int8_t pin_t;


/******************** BUTTONS **********************/
enum
{
	BUTTON_NORMAL = 0,
	BUTTON_TOGGLE,
	TOGGLE_SWITCH,
	TOGGLE_SWITCH_ON,
	TOGGLE_SWITCH_OFF,
	
	POV1_UP,
	POV1_RIGHT,
	POV1_DOWN,
	POV1_LEFT,
	POV1_CENTER,
	
	POV2_UP,
	POV2_RIGHT,
	POV2_DOWN,
	POV2_LEFT,
	POV2_CENTER,
	
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
	
	RADIO_BUTTON1,
	RADIO_BUTTON2,
	RADIO_BUTTON3,
	RADIO_BUTTON4,
	
	SEQUENTIAL_TOGGLE,
	SEQUENTIAL_BUTTON,	
};
typedef uint8_t button_type_t;

enum
{
	BUTTON_TIMER_OFF = 0,
	BUTTON_TIMER_1,
	BUTTON_TIMER_2,
	BUTTON_TIMER_3,
	
};
typedef uint8_t button_timer_t;

typedef struct button_t
{
	int8_t					physical_num;
	button_type_t 	type : 5;
	uint8_t					shift_modificator : 3;
	
	uint8_t					is_inverted :1;
	uint8_t					is_disabled :1;
	button_timer_t	delay_timer :3;
	button_timer_t	press_timer :3;
	
}	button_t;

typedef struct physical_buttons_state_t
{
  uint32_t time_last;	
	uint8_t pin_state						:1;
	uint8_t prev_pin_state			:1;
	uint8_t current_state				:1;
	uint8_t changed							:1;
	
} //.
physical_buttons_state_t;

enum
{
	BUTTON_ACTION_IDLE = 0,
	BUTTON_ACTION_DELAY,
	BUTTON_ACTION_PRESS,
	BUTTON_ACTION_BLOCK,
	
};
typedef uint8_t button_action_t;

typedef struct logical_buttons_state_t
{
  uint32_t time_last;	
	uint8_t curr_physical_state		:1;
	uint8_t prev_physical_state		:1;	
	uint8_t on_state 							:1;	
	uint8_t off_state 						:1;	
	uint8_t current_state					:1;	
	uint8_t delay_act 						:2;	
	
} logical_buttons_state_t;


/******************** ENCODERS **********************/
enum
{
	ENCODER_CONF_1x = 0,
	ENCODER_CONF_2x,
	ENCODER_CONF_4x,
	
};
typedef uint8_t encoder_t;


typedef struct
{
  uint32_t 				time_last;
	int32_t 				cnt;
	uint8_t 				state;					//:4?	
	int8_t 					pin_a;
	int8_t 					pin_b;
	int8_t					dir :4;					//:2?
	int8_t					last_dir :4;		//:2?
	
	
} encoder_state_t;


/******************** AXIS TO BUTTONS **********************/
typedef struct
{
	uint8_t points[13];
	uint8_t buttons_cnt;									// :4

} axis_to_buttons_t;


/******************** SHIFT REGISTERS **********************/
enum
{
	HC165_PULL_DOWN = 0,
	CD4021_PULL_DOWN,
	HC165_PULL_UP,
	CD4021_PULL_UP,
};	
typedef uint8_t shift_reg_config_type_t;

typedef struct
{	
	uint8_t 			type;
	uint8_t 			button_cnt;	
	int8_t 				pin_latch;
	int8_t 				pin_data;
	int8_t 				pin_clk;
	
} shift_reg_t;

typedef struct
{	
	uint8_t 			type;
	uint8_t 			button_cnt;	
	int8_t 				reserved[2];
	
} shift_reg_config_t;


/******************** SHIFT MODIFICATORS **********************/
typedef struct 
{
	int8_t 				button;
	
} shift_modificator_t;


/******************** LEDS **********************/
enum 
{
	LED_NORMAL = 0,
	LED_INVERTED,
};

typedef struct
{
	uint8_t				duty_cycle;	
	uint8_t				axis_num : 3;
	uint8_t				is_axis : 1;
	uint8_t 			:0;
	
} led_pwm_config_t;

typedef struct
{
	int8_t				input_num;
	uint8_t				type: 3;
	uint8_t				:0;
	
} led_config_t;

enum 
{
	WS2812B_STATIC = 0,
	WS2812B_SIMHUB,
	WS2812B_RAINBOW,
	WS2812B_FLOW,
};

struct RGB
{
    uint8_t r, g, b;
};

struct HSV
{
    int16_t h;
    uint8_t s, v;
};

typedef struct RGB RGB_t;
typedef struct HSV HSV_t;



/******************** DEVICE CONFIGURATION **********************/
typedef struct 
{
	// config 1
	uint16_t 						firmware_version;
	char 								device_name[26];
	uint16_t						button_debounce_ms;
	uint8_t							encoder_press_time_ms;
	uint8_t 						exchange_period_ms;
	pin_t 							pins[USED_PINS_NUM];
	
	// config 2-5
	axis_config_t 			axis_config[MAX_AXIS_NUM];
	
	// config 6-7-8-9-10-11-12
	button_t 						buttons[MAX_BUTTONS_NUM];
	uint16_t						button_timer1_ms;						// config packet 6				
	uint16_t						button_timer2_ms;						// config packet 7
	uint16_t						button_timer3_ms;						// config packet 8
	uint16_t 						a2b_debounce_ms;						// config packet 9
	
	// config 12-13-14
	axis_to_buttons_t		axes_to_buttons[MAX_AXIS_NUM];
	
	// config 14	
	shift_reg_config_t	shift_registers[4];
	shift_modificator_t	shift_config[5];
	uint16_t						vid;
	uint16_t						pid;
	
	// config 15;
	led_pwm_config_t		led_pwm_config[4];
	led_config_t				leds[MAX_LEDS_NUM];
	
	// config 16;
	encoder_t						encoders[MAX_ENCODERS_NUM];
	
	uint8_t							button_polling_interval_ticks;
	uint8_t							encoder_polling_interval_ticks;
	
	uint8_t							rgb_effect;
	uint8_t							rgb_count;
	uint8_t							rgb_brightness;
	uint16_t						rgb_delay_ms;
	RGB_t 							rgb_leds[NUM_RGB_LEDS];
	
}dev_config_t;


/******************** APPLICATION CONFIGURATION **********************/
typedef struct
{
	uint8_t							axis;
	uint8_t 						axis_cnt;
	uint8_t							buttons_cnt;
	uint8_t							pov;
	uint8_t							pov_cnt;
	uint8_t							slow_encoder_cnt;
	uint8_t							fast_encoder_cnt;
	uint8_t							pwm_cnt;
	uint8_t							rgb_cnt;
	
} app_config_t;


/******************** HID REPORT CONFIGURATION **********************/
typedef struct
{
	analog_data_t			 	axis_data[MAX_AXIS_NUM];
	uint8_t 						pov_data[MAX_POVS_NUM];
	uint8_t 						button_data[MAX_BUTTONS_NUM/8];
	
} joy_report_t;

typedef struct
{
	uint16_t 						firmware_version;
	analog_data_t				raw_axis_data[MAX_AXIS_NUM];
	analog_data_t			 	axis_data[MAX_AXIS_NUM];
	uint8_t							phy_button_data[MAX_BUTTONS_NUM/8];
	uint8_t							log_button_data[MAX_BUTTONS_NUM/8];
	uint8_t							shift_button_data;
	
} params_report_t;



#endif 	/* __COMMON_TYPES_H__ */
