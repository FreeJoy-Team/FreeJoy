/**
  ******************************************************************************
  * @file           : common_defines.h
  * @brief          : This file contains the common defines for the app.                  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_DEFINES_H__
#define __COMMON_DEFINES_H__

#define DEBUG

#define FIRMWARE_VERSION					0x1321			// v1.3.2b1
#define USED_PINS_NUM							30					// constant for BluePill and BlackPill boards
#define MAX_AXIS_NUM							8						// max 8
#define MAX_BUTTONS_NUM						128					// power of 2, max 128
#define MAX_POVS_NUM							4						// max 4
#define MAX_ENCODERS_NUM					16					// max 64
#define MAX_SHIFT_REG_NUM					4						// max 4

#define AXIS_MIN_VALUE						(-32767)
#define AXIS_MAX_VALUE						(32767)
#define AXIS_CENTER_VALUE					(AXIS_MIN_VALUE + (AXIS_MAX_VALUE-AXIS_MIN_VALUE)/2)
#define AXIS_FULLSCALE						(AXIS_MAX_VALUE - AXIS_MIN_VALUE)

#define CONFIG_ADDR								(0x0800FC00)
#define FIRMWARE_COPY_ADDR				(0x8000000 + 0x7000)
#define BOOTLOADER_ADDR						(0x0800F400)

enum
{
	REPORT_ID_JOY = 1,
	REPORT_ID_CONFIG_IN,
	REPORT_ID_CONFIG_OUT,
	REPORT_ID_FIRMWARE,
};


#endif 	/* __COMMON_DEFINES_H__ */
