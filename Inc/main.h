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
	.firmware_version = 1,
	
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
	
	.axis_config[0].calib_min = 0,
	.axis_config[0].calib_center = 2047,
	.axis_config[0].calib_max = 4095,
	.axis_config[0].autocalib = 0,
	.axis_config[0].curve_shape.point1 = 0,
	.axis_config[0].curve_shape.point2 = 0,
	.axis_config[0].curve_shape.point3 = 0,
	.axis_config[0].curve_shape.point4 = 0,
	.axis_config[0].curve_shape.point5 = 0,
	.axis_config[0].curve_shape.point6 = 0,
	.axis_config[0].curve_shape.point7 = 0,
	.axis_config[0].curve_shape.point8 = 0,
	.axis_config[0].curve_shape.point9 = 0,
	.axis_config[0].curve_shape.point10 = 0,
	.axis_config[0].filter = FILTER_NO,
	
	.axis_config[1].calib_min = 0,
	.axis_config[1].calib_center = 2047,
	.axis_config[1].calib_max = 4095,
	.axis_config[1].autocalib = 0,
	.axis_config[1].curve_shape.point1 = 0,
	.axis_config[1].curve_shape.point2 = 0,
	.axis_config[1].curve_shape.point3 = 0,
	.axis_config[1].curve_shape.point4 = 0,
	.axis_config[1].curve_shape.point5 = 0,
	.axis_config[1].curve_shape.point6 = 0,
	.axis_config[1].curve_shape.point7 = 0,
	.axis_config[1].curve_shape.point8 = 0,
	.axis_config[1].curve_shape.point9 = 0,
	.axis_config[1].curve_shape.point10 = 0,
	.axis_config[1].filter = FILTER_NO,
	
	.axis_config[2].calib_min = 0,
	.axis_config[2].calib_center = 2047,
	.axis_config[2].calib_max = 4095,
	.axis_config[2].autocalib = 0,
	.axis_config[2].curve_shape.point1 = 0,
	.axis_config[2].curve_shape.point2 = 0,
	.axis_config[2].curve_shape.point3 = 0,
	.axis_config[2].curve_shape.point4 = 0,
	.axis_config[2].curve_shape.point5 = 0,
	.axis_config[2].curve_shape.point6 = 0,
	.axis_config[2].curve_shape.point7 = 0,
	.axis_config[2].curve_shape.point8 = 0,
	.axis_config[2].curve_shape.point9 = 0,
	.axis_config[2].curve_shape.point10 = 0,
	.axis_config[2].filter = FILTER_NO,
	
	.axis_config[3].calib_min = 0,
	.axis_config[3].calib_center = 2047,
	.axis_config[3].calib_max = 4095,
	.axis_config[3].autocalib = 0,
	.axis_config[3].curve_shape.point1 = 0,
	.axis_config[3].curve_shape.point2 = 0,
	.axis_config[3].curve_shape.point3 = 0,
	.axis_config[3].curve_shape.point4 = 0,
	.axis_config[3].curve_shape.point5 = 0,
	.axis_config[3].curve_shape.point6 = 0,
	.axis_config[3].curve_shape.point7 = 0,
	.axis_config[3].curve_shape.point8 = 0,
	.axis_config[3].curve_shape.point9 = 0,
	.axis_config[3].curve_shape.point10 = 0,
	.axis_config[3].filter = FILTER_NO,
	
	.axis_config[4].calib_min = 0,
	.axis_config[4].calib_center = 2047,
	.axis_config[4].calib_max = 4095,
	.axis_config[4].autocalib = 0,
	.axis_config[4].curve_shape.point1 = 0,
	.axis_config[4].curve_shape.point2 = 0,
	.axis_config[4].curve_shape.point3 = 0,
	.axis_config[4].curve_shape.point4 = 0,
	.axis_config[4].curve_shape.point5 = 0,
	.axis_config[4].curve_shape.point6 = 0,
	.axis_config[4].curve_shape.point7 = 0,
	.axis_config[4].curve_shape.point8 = 0,
	.axis_config[4].curve_shape.point9 = 0,
	.axis_config[4].curve_shape.point10 = 0,
	.axis_config[4].filter = FILTER_NO,
	
	.axis_config[5].calib_min = 0,
	.axis_config[5].calib_center = 2047,
	.axis_config[5].calib_max = 4095,
	.axis_config[5].autocalib = 0,
	.axis_config[5].curve_shape.point1 = 0,
	.axis_config[5].curve_shape.point2 = 0,
	.axis_config[5].curve_shape.point3 = 0,
	.axis_config[5].curve_shape.point4 = 0,
	.axis_config[5].curve_shape.point5 = 0,
	.axis_config[5].curve_shape.point6 = 0,
	.axis_config[5].curve_shape.point7 = 0,
	.axis_config[5].curve_shape.point8 = 0,
	.axis_config[5].curve_shape.point9 = 0,
	.axis_config[5].curve_shape.point10 = 0,
	.axis_config[5].filter = FILTER_NO,
	
	.axis_config[6].calib_min = 0,
	.axis_config[6].calib_center = 2047,
	.axis_config[6].calib_max = 4095,
	.axis_config[6].autocalib = 0,
	.axis_config[6].curve_shape.point1 = 0,
	.axis_config[6].curve_shape.point2 = 0,
	.axis_config[6].curve_shape.point3 = 0,
	.axis_config[6].curve_shape.point4 = 0,
	.axis_config[6].curve_shape.point5 = 0,
	.axis_config[6].curve_shape.point6 = 0,
	.axis_config[6].curve_shape.point7 = 0,
	.axis_config[6].curve_shape.point8 = 0,
	.axis_config[6].curve_shape.point9 = 0,
	.axis_config[6].curve_shape.point10 = 0,
	.axis_config[6].filter = FILTER_NO,
	
	.axis_config[7].calib_min = 0,
	.axis_config[7].calib_center = 2047,
	.axis_config[7].calib_max = 4095,
	.axis_config[7].autocalib = 0,
	.axis_config[7].curve_shape.point1 = 0,
	.axis_config[7].curve_shape.point2 = 0,
	.axis_config[7].curve_shape.point3 = 0,
	.axis_config[7].curve_shape.point4 = 0,
	.axis_config[7].curve_shape.point5 = 0,
	.axis_config[7].curve_shape.point6 = 0,
	.axis_config[7].curve_shape.point7 = 0,
	.axis_config[7].curve_shape.point8 = 0,
	.axis_config[7].curve_shape.point9 = 0,
	.axis_config[7].curve_shape.point10 = 0,
	.axis_config[7].filter = FILTER_NO,
	
	.buttons[0] = BUTTON_NORMAL,
	.buttons[1] = BUTTON_INVERTED,
	.buttons[2] = BUTTON_TOGGLE,
	.buttons[3] = TOGGLE_SWITCH,
	.buttons[4] = TOGGLE_SWITCH_ON,
	.buttons[5] = TOGGLE_SWITCH_OFF,
	.buttons[6] = BUTTON_NORMAL,
	.buttons[7] = BUTTON_NORMAL,
/*.
	.
	.buttons[127] = BUTTON_NORMAL,
*/

	
	.encoders[0].pin_a = 0xFF,
	.encoders[0].pin_b = 0xFF,
	.encoders[0].pin_c = 0xFF,
	.encoders[0].type = ENCODER_1_1,
	
	.encoders[1].pin_a = 0xFF,
	.encoders[1].pin_b = 0xFF,
	.encoders[1].pin_c = 0xFF,
	.encoders[1].type = ENCODER_1_1,
	
	.encoders[2].pin_a = 0xFF,
	.encoders[2].pin_b = 0xFF,
	.encoders[2].pin_c = 0xFF,
	.encoders[2].type = ENCODER_1_1,
	
	.encoders[3].pin_a = 0xFF,
	.encoders[3].pin_b = 0xFF,
	.encoders[3].pin_c = 0xFF,
	.encoders[3].type = ENCODER_1_1,
	
	.encoders[4].pin_a = 0xFF,
	.encoders[4].pin_b = 0xFF,
	.encoders[4].pin_c = 0xFF,
	.encoders[4].type = ENCODER_1_1,
	
	.encoders[5].pin_a = 0xFF,
	.encoders[5].pin_b = 0xFF,
	.encoders[5].pin_c = 0xFF,
	.encoders[5].type = ENCODER_1_1,
	
	.encoders[6].pin_a = 0xFF,
	.encoders[6].pin_b = 0xFF,
	.encoders[6].pin_c = 0xFF,
	.encoders[6].type = ENCODER_1_1,
	
	.encoders[7].pin_a = 0,
	.encoders[7].pin_b = 1,
	.encoders[7].pin_c = 0xFF,
	.encoders[7].type = ENCODER_1_1,
	
	.encoders[8].pin_a = 0xFF,
	.encoders[8].pin_b = 0xFF,
	.encoders[8].pin_c = 0xFF,
	.encoders[8].type = ENCODER_1_1,
	
	.encoders[9].pin_a = 0,
	.encoders[9].pin_b = 1,
	.encoders[9].pin_c = 0xFF,
	.encoders[9].type = ENCODER_1_1,
	
	.encoders[10].pin_a = 0xFF,
	.encoders[10].pin_b = 0xFF,
	.encoders[10].pin_c = 0xFF,
	.encoders[10].type = ENCODER_1_1,
	
	.encoders[11].pin_a = 0xFF,
	.encoders[11].pin_b = 0xFF,
	.encoders[11].pin_c = 0xFF,
	.encoders[11].type = ENCODER_1_1,
	
	.encoders[12].pin_a = 0xFF,
	.encoders[12].pin_b = 0xFF,
	.encoders[12].pin_c = 0xFF,
	.encoders[12].type = ENCODER_1_1,
	
	.encoders[13].pin_a = 0xFF,
	.encoders[13].pin_b = 0xFF,
	.encoders[13].pin_c = 0xFF,
	.encoders[13].type = ENCODER_1_1,
	
	.encoders[14].pin_a = 0xFF,
	.encoders[14].pin_b = 0xFF,
	.encoders[14].pin_c = 0xFF,
	.encoders[14].type = ENCODER_1_1,
	
	.encoders[15].pin_a = 0xFF,
	.encoders[15].pin_b = 0xFF,
	.encoders[15].pin_c = 0xFF,
	.encoders[15].type = ENCODER_1_1,

	.hid_id[0] = 'F',
	.hid_id[1] = 'r',
	.hid_id[2] = 'e',
	.hid_id[3] = 'e',
	.hid_id[4] = 'J',
	.hid_id[5] = 'o',
	.hid_id[6] = 'y',
	.hid_id[7] = ' ',
	.hid_id[8] = ' ',
	.hid_id[9] = 0,
	
	.button_debounce_ms = 30,
	.toggle_press_time_ms = 200,
	.encoder_press_time_ms = 100,
	
};

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

