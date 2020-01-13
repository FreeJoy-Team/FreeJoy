/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "flash.h"
#include "crc16.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

extern app_config_t config;
volatile extern uint8_t config_in_cnt;
volatile extern uint8_t config_out_cnt;
volatile extern uint16_t firmware_in_cnt;
volatile extern uint8_t bootloader;
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
		0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)

		0x85, REPORT_ID_JOY,				 	 //		REPORT_ID	(JOY_REPORT_ID)	
	  0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, MAX_BUTTONS_NUM,         //   USAGE_MAXIMUM (Button MAX_BUTTONS_NUM)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, MAX_BUTTONS_NUM,         //   REPORT_COUNT (MAX_BUTTONS_NUM)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)

		0x05, 0x01,                    // 	USAGE_PAGE (Generic Desktop)
		0x09, 0x30,                    //   USAGE (X)
    0x09, 0x31,                    //   USAGE (Y)
    0x09, 0x32,                    //   USAGE (Z)
    0x09, 0x33,                    //   USAGE (Rx)
    0x09, 0x34,                    //   USAGE (Ry)
    0x09, 0x35,                    //   USAGE (Rz)
		0x09, 0x36,                    //   USAGE (Slider)
		0x09, 0x36,                    //  	USAGE (Slider)
    0x15, 0x00,                    //  	LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x0F,              //   LOGICAL_MAXIMUM (4095)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, MAX_AXIS_NUM,       		 //   REPORT_COUNT (MAX_AXIS_NUM)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
		
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x15, 0x00, 									 //   LOGICAL_MINIMUM (0)
		0x25, 0x07, 									 //   LOGICAL_MAXIMUM (7)
		0x35, 0x00, 									 //   PHYSICAL_MINIMUM (0)
		0x46, 0x3B, 0x01,							 //   PHYSICAL_MAXIMUM (315)
		0x65, 0x12, 									 //   UNIT (SI Rot:Angular Pos) 
		0x75, 0x08, 									 //   REPORT_SIZE (8) 
		0x95, 0x01, 								   //   REPORT_COUNT (1)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		
		// raw axis data
		0x06, 0x00, 0xff,              // 	USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (4095)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, MAX_AXIS_NUM,            //   REPORT_COUNT (MAX_AXIS_NUM)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
		// config data
		0x85, REPORT_ID_CONFIG_IN,     //   REPORT_ID (2)
    0x09, 0x02,                    //   USAGE (Vendor Usage 2)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x03,                    //   USAGE (Vendor Usage 3)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
    0x85, REPORT_ID_CONFIG_OUT,    //   REPORT_ID (3)	
    0x09, 0x04,                    //   USAGE (Vendor Usage 4)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
		0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x05,                    //   USAGE (Vendor Usage 5)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
		0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
		0x85, REPORT_ID_FIRMWARE,    	 //   REPORT_ID (3)	
    0x09, 0x06,                    //   USAGE (Vendor Usage 6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x02,                    //   REPORT_COUNT (1)
		0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x07,                    //   USAGE (Vendor Usage 7)
    0x75, 0x08,                    //   REPORT_SIZE (8)
		0x95, 0x3f,                    //   REPORT_COUNT (63)
		0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
		0xc0,                           // END_COLLECTION

};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
	static app_config_t tmp_config;
	static uint16_t firmware_len = 0;
	uint8_t i;
	uint8_t pos = 2;
	uint8_t repotId;
	USBD_CUSTOM_HID_HandleTypeDef * hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;
	
	repotId = hhid->Report_buf[0];
	
	switch (repotId)
	{
		case REPORT_ID_CONFIG_IN:
		{
			config_in_cnt = hhid->Report_buf[1];			// requested config packet number
		}
		break;
		
		case REPORT_ID_CONFIG_OUT:
		{
			switch (hhid->Report_buf[1])
			{
				case 1:
					{
//					memcpy((uint8_t *) &(tmp_config.firmware_version), &hhid->Report_buf[pos], sizeof(tmp_config.firmware_version));
						pos += sizeof(tmp_config.firmware_version);
						memcpy((uint8_t *) &(tmp_config.device_name), &hhid->Report_buf[pos], sizeof(tmp_config.device_name));
						pos += sizeof(tmp_config.device_name);
						memcpy((uint8_t *) &(tmp_config.button_debounce_ms), &hhid->Report_buf[pos], 8);
						pos += 8;
						
						memcpy((uint8_t *) &(tmp_config.pins), &hhid->Report_buf[63-sizeof(tmp_config.pins)], sizeof(tmp_config.pins));
					}
				break;
				
				case 2:
				{
					i = 0;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hhid->Report_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;
				
				case 3:
				{
					i = 2;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hhid->Report_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;
				
				case 4:
				{
					i = 4;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hhid->Report_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;

				case 5:
				{
					i = 6;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hhid->Report_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;
				
				case 6:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[0]), &hhid->Report_buf[pos], 62);
				}
				break;
				
				case 7:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[62]), &hhid->Report_buf[pos], 62);
				}
				break;
				
				case 8:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[124]), &hhid->Report_buf[pos], 4);
					pos += 4;
					i = 0;
					while(64 - pos > sizeof(axis_to_buttons_t))
					{
						memcpy((uint8_t *) &(tmp_config.axes_to_buttons[i++]), &hhid->Report_buf[pos], sizeof(axis_to_buttons_t));
						pos += sizeof(axis_to_buttons_t);
					}
				}
				break;
				
				case 9:
				{
					i = 4;
					while(64 - pos > sizeof(axis_to_buttons_t))
					{
						memcpy((uint8_t *) &(tmp_config.axes_to_buttons[i++]), &hhid->Report_buf[pos], sizeof(axis_to_buttons_t));
						pos += sizeof(axis_to_buttons_t);
					}
				}					
					break;
				
				case 10:
				{
					for (i=0; i<4; i++)
					{
						memcpy((uint8_t *) &(tmp_config.shift_registers[i]), &hhid->Report_buf[pos], sizeof(shift_reg_config_t));
						pos += sizeof(shift_reg_config_t);
					}
				}					
					break;
				
				default:
					break;
			}
			if (hhid->Report_buf[1] < 10)		// request new packet
			{
				config_out_cnt = hhid->Report_buf[1] + 1;
			}
			else // last packet received
			{
				tmp_config.firmware_version = config.firmware_version;
				ConfigSet(&tmp_config);
				
				//ConfigGet(&config);	
				HAL_NVIC_SystemReset();
			}
		}
		break;
			
		case REPORT_ID_FIRMWARE:
		{
			FLASH_EraseInitTypeDef FLASH_EraseInitStruct;
			uint32_t PageError = 0;
			uint16_t crc_in = 0;
			uint16_t crc_comp = 0;
			
			uint16_t cnt = hhid->Report_buf[1]<<8 | hhid->Report_buf[2];
			
			if (cnt == 0)			// first packet with info data
			{
				firmware_len = hhid->Report_buf[5]<<8 | hhid->Report_buf[4];
				
				if (firmware_len <= 0x6400)	// check new firmware size
				{
				
				FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
				FLASH_EraseInitStruct.NbPages = 24;
				FLASH_EraseInitStruct.PageAddress = FIRMWARE_COPY_ADDR;
				
				HAL_FLASH_Unlock();
				HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &PageError);
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hhid->Report_buf[i + 5]<<8 | hhid->Report_buf[i + 4];
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
				}
				HAL_FLASH_Lock();
				firmware_in_cnt = cnt+1;
				}
				else // firmware size error
				{
					
				}
			}
			else if ( (firmware_len > 0) && (cnt*60 < firmware_len) )		// body of firmware data
			{
				HAL_FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hhid->Report_buf[i + 5]<<8 | hhid->Report_buf[i + 4];
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
				}
				HAL_FLASH_Lock();
				firmware_in_cnt = cnt+1;
			}
			else if (firmware_len > 0)		// last packet
			{
				HAL_FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hhid->Report_buf[i + 5]<<8 | hhid->Report_buf[i + 4];
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
				}
				HAL_FLASH_Lock();
				
				// check CRC16
				crc_in = (*(uint8_t *) (FIRMWARE_COPY_ADDR + 3)) << 8 | (*(uint8_t *) (FIRMWARE_COPY_ADDR + 2));				
				crc_comp = Crc16((uint8_t*)FIRMWARE_COPY_ADDR + 60, firmware_len);				
				if (crc_in == crc_comp && crc_comp != 0)
				{
					bootloader = 1;
				}
			}
			else break;
			
			
			
		}
		break;
		
		default:
			break;
	}
	
  return (USBD_OK);
  /* USER CODE END 6 */
 }

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

