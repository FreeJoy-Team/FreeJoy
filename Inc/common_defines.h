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

#define FIRMWARE_VERSION				0x0001
#define USED_PINS_NUM						30					// constant for BluePill and BlackPill boards
#define MAX_AXIS_NUM						8						// max 8
#define MAX_BUTTONS_NUM					128					// power of 2, max 128
#define MAX_POVS_NUM						4						// max 4
#define MAX_ENCODERS_NUM				12					// max 16


#define JOY_REPORT_ID						1
#define CONFIG_REPORT_ID				2


#endif 	/* __COMMON_DEFINES_H__ */
