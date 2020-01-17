/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "common_types.h"


static const app_config_t init_config =
{
	.firmware_version = 0x0102,		// do not change
	
	/* 
		Name of device in devices dispatcher
	*/
	.device_name[0] = 'F',
	.device_name[1] = 'r',
	.device_name[2] = 'e',
	.device_name[3] = 'e',
	.device_name[4] = 'J',
	.device_name[5] = 'o',
	.device_name[6] = 'y',
	.device_name[7] = 0,
	.device_name[8] = 0,
	.device_name[9] = 0,
	.device_name[10] = 0,
	.device_name[11] = 0,
	.device_name[12] = 0,
	.device_name[13] = 0,
	.device_name[14] = 0,
	.device_name[15] = 0,
	.device_name[16] = 0,
	.device_name[17] = 0,
	.device_name[18] = 0,
	.device_name[19] = 0,			
	
	.button_debounce_ms = 30,					// debounce time for all buttons
	
	.toggle_press_time_ms = 200,			// amount of millisecods virtual button 
																		// will be pressed at toggle switch event
	
	.encoder_press_time_ms = 30,			// amount of millisecods virtual button 
																		// will be pressed at encoder increment/decrement
																		
	.exchange_period_ms = 2,					// amount of millisecond between joystick data sending

	/*
		Device pins configuration. Available values:
		- AXIS_ANALOG (only for pins 0-7)
		- BUTTON_GND
		- BUTTON_VCC
		- BUTTON_COLUMN
		- BUTTON_ROW
	*/
	.pins[0] = AXIS_ANALOG,						// PA0
	.pins[1] = AXIS_ANALOG,						// PA1
	.pins[2] = AXIS_ANALOG,						// PA2
	.pins[3] = AXIS_ANALOG,						// PA3
	.pins[4] = AXIS_ANALOG,						// PA4
	.pins[5] = AXIS_ANALOG,						// PA5
	.pins[6] = AXIS_ANALOG,						// PA6
	.pins[7] = AXIS_ANALOG,						// PA7
	.pins[8] = BUTTON_GND,						// PA8
	.pins[9] = BUTTON_GND,						// PA9
	.pins[10] = BUTTON_GND,						// PA10
	.pins[11] = NOT_USED,							// PA15
	.pins[12] = NOT_USED,							// PB0
	.pins[13] = NOT_USED,							// PB1
	.pins[14] = BUTTON_GND,						// PB3
	.pins[15] = BUTTON_GND,						// PB4
	.pins[16] = BUTTON_GND,						// PB5
	.pins[17] = BUTTON_GND,						// PB6
	.pins[18] = BUTTON_GND,						// PB7
	.pins[19] = NOT_USED,							// PB8
	.pins[20] = NOT_USED,							// PB9
	.pins[21] = NOT_USED,							// PB10
	.pins[22] = NOT_USED,							// PB11
	.pins[23] = BUTTON_GND,						// PB12
	.pins[24] = BUTTON_GND,						// PB13
	.pins[25] = BUTTON_GND,						// PB14
	.pins[26] = BUTTON_GND,						// PB15
	.pins[27] = NOT_USED,							// PC13
	.pins[28] = NOT_USED,							// PC14
	.pins[29] = NOT_USED,							// PC15
	
	/*
		Configuration of analog axes
	*/
	.axis_config[0].calib_min = 0,
	.axis_config[0].calib_center = 2047,
	.axis_config[0].calib_max = 4095,
	.axis_config[0].autocalib = 0,
	.axis_config[0].curve_shape[0] = -100,
	.axis_config[0].curve_shape[1]  = -77,
	.axis_config[0].curve_shape[2] = -55,
	.axis_config[0].curve_shape[3] = -33,
	.axis_config[0].curve_shape[4] = -11,
	.axis_config[0].curve_shape[5] = 11,
	.axis_config[0].curve_shape[6] = 33,
	.axis_config[0].curve_shape[7] = 55,
	.axis_config[0].curve_shape[8] = 77,
	.axis_config[0].curve_shape[9] = 100,
	.axis_config[0].filter = FILTER_NO,
	.axis_config[0].out_enabled = 1,
	.axis_config[0].resolution = 12,
	
	.axis_config[1].calib_min = 0,
	.axis_config[1].calib_center = 2047,
	.axis_config[1].calib_max = 4095,
	.axis_config[1].autocalib = 0,
	.axis_config[1].curve_shape[0] = -100,
	.axis_config[1].curve_shape[1]  = -77,
	.axis_config[1].curve_shape[2] = -55,
	.axis_config[1].curve_shape[3] = -33,
	.axis_config[1].curve_shape[4] = -11,
	.axis_config[1].curve_shape[5] = 11,
	.axis_config[1].curve_shape[6] = 33,
	.axis_config[1].curve_shape[7] = 55,
	.axis_config[1].curve_shape[8] = 77,
	.axis_config[1].curve_shape[9] = 100,
	.axis_config[1].filter = FILTER_NO,
	.axis_config[1].out_enabled = 1,
	.axis_config[1].resolution = 12,
	
	.axis_config[2].calib_min = 0,
	.axis_config[2].calib_center = 2047,
	.axis_config[2].calib_max = 4095,
	.axis_config[2].autocalib = 0,
	.axis_config[2].curve_shape[0] = -100,
	.axis_config[2].curve_shape[1]  = -77,
	.axis_config[2].curve_shape[2] = -55,
	.axis_config[2].curve_shape[3] = -33,
	.axis_config[2].curve_shape[4] = -11,
	.axis_config[2].curve_shape[5] = 11,
	.axis_config[2].curve_shape[6] = 33,
	.axis_config[2].curve_shape[7] = 55,
	.axis_config[2].curve_shape[8] = 77,
	.axis_config[2].curve_shape[9] = 100,
	.axis_config[2].filter = FILTER_NO,
	.axis_config[2].out_enabled = 1,
	.axis_config[2].resolution = 12,
	
	.axis_config[3].calib_min = 0,
	.axis_config[3].calib_center = 2047,
	.axis_config[3].calib_max = 4095,
	.axis_config[3].autocalib = 0,
	.axis_config[3].curve_shape[0] = -100,
	.axis_config[3].curve_shape[1]  = -77,
	.axis_config[3].curve_shape[2] = -55,
	.axis_config[3].curve_shape[3] = -33,
	.axis_config[3].curve_shape[4] = -11,
	.axis_config[3].curve_shape[5] = 11,
	.axis_config[3].curve_shape[6] = 33,
	.axis_config[3].curve_shape[7] = 55,
	.axis_config[3].curve_shape[8] = 77,
	.axis_config[3].curve_shape[9] = 100,
	.axis_config[3].filter = FILTER_NO,
	.axis_config[3].out_enabled = 1,
	.axis_config[3].resolution = 12,
	
	.axis_config[4].calib_min = 0,
	.axis_config[4].calib_center = 2047,
	.axis_config[4].calib_max = 4095,
	.axis_config[4].autocalib = 0,
	.axis_config[4].curve_shape[0] = -100,
	.axis_config[4].curve_shape[1]  = -77,
	.axis_config[4].curve_shape[2] = -55,
	.axis_config[4].curve_shape[3] = -33,
	.axis_config[4].curve_shape[4] = -11,
	.axis_config[4].curve_shape[5] = 11,
	.axis_config[4].curve_shape[6] = 33,
	.axis_config[4].curve_shape[7] = 55,
	.axis_config[4].curve_shape[8] = 77,
	.axis_config[4].curve_shape[9] = 100,
	.axis_config[4].filter = FILTER_NO,
	.axis_config[4].out_enabled = 1,
	.axis_config[4].resolution = 12,
	
	.axis_config[5].calib_min = 0,
	.axis_config[5].calib_center = 2047,
	.axis_config[5].calib_max = 4095,
	.axis_config[5].autocalib = 0,
	.axis_config[5].curve_shape[0] = -100,
	.axis_config[5].curve_shape[1]  = -77,
	.axis_config[5].curve_shape[2] = -55,
	.axis_config[5].curve_shape[3] = -33,
	.axis_config[5].curve_shape[4] = -11,
	.axis_config[5].curve_shape[5] = 11,
	.axis_config[5].curve_shape[6] = 33,
	.axis_config[5].curve_shape[7] = 55,
	.axis_config[5].curve_shape[8] = 77,
	.axis_config[5].curve_shape[9] = 100,
	.axis_config[5].filter = FILTER_NO,
	.axis_config[5].out_enabled = 1,
	.axis_config[5].resolution = 12,
	
	.axis_config[6].calib_min = 0,
	.axis_config[6].calib_center = 2047,
	.axis_config[6].calib_max = 4095,
	.axis_config[6].autocalib = 0,
	.axis_config[6].curve_shape[0] = -100,
	.axis_config[6].curve_shape[1]  = -77,
	.axis_config[6].curve_shape[2] = -55,
	.axis_config[6].curve_shape[3] = -33,
	.axis_config[6].curve_shape[4] = -11,
	.axis_config[6].curve_shape[5] = 11,
	.axis_config[6].curve_shape[6] = 33,
	.axis_config[6].curve_shape[7] = 55,
	.axis_config[6].curve_shape[8] = 77,
	.axis_config[6].curve_shape[9] = 100,
	.axis_config[6].filter = FILTER_NO,
	.axis_config[6].out_enabled = 1,
	.axis_config[6].resolution = 12,
	
	.axis_config[7].calib_min = 0,
	.axis_config[7].calib_center = 2047,
	.axis_config[7].calib_max = 4095,
	.axis_config[7].autocalib = 0,
	.axis_config[7].curve_shape[0] = -100,
	.axis_config[7].curve_shape[1]  = -77,
	.axis_config[7].curve_shape[2] = -55,
	.axis_config[7].curve_shape[3] = -33,
	.axis_config[7].curve_shape[4] = -11,
	.axis_config[7].curve_shape[5] = 11,
	.axis_config[7].curve_shape[6] = 33,
	.axis_config[7].curve_shape[7] = 55,
	.axis_config[7].curve_shape[8] = 77,
	.axis_config[7].curve_shape[9] = 100,
	.axis_config[7].filter = FILTER_NO,
	.axis_config[7].out_enabled = 1,
	.axis_config[7].resolution = 12,
	
	/*
		Buttons configuration. Available values:
		- BUTTON_NORMAL
		- BUTTON_INVERTED
		- BUTTON_TOGGLE
		- TOGGLE_SWITCH
		- TOGGLE_SWITCH_ON
		- TOGGLE_SWITCH_OFF
			
		- POV1_UP
		- POV1_RIGHT
		- POV1_DOWN
		- POV1_LEFT
		- POV2_UP
		- POV2_RIGHT
		- POV2_DOWN
		- POV2_LEFT
		- POV3_UP
		- POV3_RIGHT
		- POV3_DOWN
		- POV3_LEFT
		- POV4_UP
		- POV4_RIGHT
		- POV4_DOWN
		- POV4_LEFT
			
		- ENCODER_INPUT_A
		- ENCODER_INPUT_B
	*/
	
	.buttons[0] = BUTTON_NORMAL,
	.buttons[1] = BUTTON_NORMAL,
	.buttons[2] = BUTTON_NORMAL,
	.buttons[3] = BUTTON_NORMAL,
	.buttons[4] = BUTTON_NORMAL,
	.buttons[5] = BUTTON_NORMAL,
	.buttons[6] = BUTTON_NORMAL,
	.buttons[7] = BUTTON_NORMAL,
/*.
	.
	.buttons[127] = BUTTON_NORMAL,
*/

	/*
		AxesToButtons configuration
	*/
	.axes_to_buttons[0].points[0] = 0,
	.axes_to_buttons[0].points[1] = 50,
	.axes_to_buttons[0].points[2] = 100,
	.axes_to_buttons[0].buttons_cnt = 2,
	.axes_to_buttons[0].is_enabled = 0,
	
	.axes_to_buttons[1].points[0] = 0,
	.axes_to_buttons[1].points[1] = 50,
	.axes_to_buttons[1].points[2] = 100,
	.axes_to_buttons[1].buttons_cnt = 2,
	.axes_to_buttons[1].is_enabled = 0,
	
	.axes_to_buttons[2].points[0] = 0,
	.axes_to_buttons[2].points[1] = 50,
	.axes_to_buttons[2].points[2] = 100,
	.axes_to_buttons[2].buttons_cnt = 2,
	.axes_to_buttons[2].is_enabled = 0,
	
	.axes_to_buttons[3].points[0] = 0,
	.axes_to_buttons[3].points[1] = 50,
	.axes_to_buttons[3].points[2] = 100,
	.axes_to_buttons[3].buttons_cnt = 2,
	.axes_to_buttons[3].is_enabled = 0,
	
	.axes_to_buttons[4].points[0] = 0,
	.axes_to_buttons[4].points[1] = 50,
	.axes_to_buttons[4].points[2] = 100,
	.axes_to_buttons[4].buttons_cnt = 2,
	.axes_to_buttons[4].is_enabled = 0,
	
	.axes_to_buttons[5].points[0] = 0,
	.axes_to_buttons[5].points[1] = 50,
	.axes_to_buttons[5].points[2] = 100,
	.axes_to_buttons[5].buttons_cnt = 2,
	.axes_to_buttons[5].is_enabled = 0,
	
	.axes_to_buttons[6].points[0] = 0,
	.axes_to_buttons[6].points[1] = 50,
	.axes_to_buttons[6].points[2] = 100,
	.axes_to_buttons[6].buttons_cnt = 2,
	.axes_to_buttons[6].is_enabled = 0,
	
	.axes_to_buttons[7].points[0] = 0,
	.axes_to_buttons[7].points[1] = 50,
	.axes_to_buttons[7].points[2] = 100,
	.axes_to_buttons[7].buttons_cnt = 2,
	.axes_to_buttons[7].is_enabled = 0,
		
};


void EnterBootloader (void);

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

