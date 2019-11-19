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
	.firmware_version = 1,		// do not change
	
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
																		
	.exchange_period_ms = 10,					// amount of millisecond between joystick data sending

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
	.pins[11] = BUTTON_GND,						// PA15
	.pins[12] = BUTTON_GND,						// PB0
	.pins[13] = BUTTON_GND,						// PB1
	.pins[14] = BUTTON_GND,						// PB3
	.pins[15] = BUTTON_GND,						// PB4
	.pins[16] = BUTTON_COLUMN,				// PB5
	.pins[17] = BUTTON_COLUMN,				// PB6
	.pins[18] = BUTTON_COLUMN,				// PB7
	.pins[19] = BUTTON_ROW,						// PB8
	.pins[20] = BUTTON_ROW,						// PB9
	.pins[21] = BUTTON_ROW,						// PB10
	.pins[22] = NOT_USED,							// PB11
	.pins[23] = NOT_USED,							// PB12
	.pins[24] = NOT_USED,							// PB13
	.pins[25] = NOT_USED,							// PB14
	.pins[26] = NOT_USED,							// PB15
	.pins[27] = NOT_USED,							// PC13
	.pins[28] = NOT_USED,							// PC14
	.pins[29] = NOT_USED,							// PC15
	
	/*
		Configuration of anlog axes
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

