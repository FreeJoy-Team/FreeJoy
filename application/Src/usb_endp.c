/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "config.h"
#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile extern uint8_t bootloader;
volatile extern int64_t joy_millis;
volatile extern int64_t encoder_ticks;
volatile extern int64_t adc_ticks;
volatile extern int64_t sensors_ticks;
volatile extern int64_t buttons_ticks;
volatile extern int64_t configurator_millis;

__IO uint8_t EP1_PrevXferComplete = 1;
__IO uint8_t EP2_PrevXferComplete = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
	SetEPRxStatus(ENDP1, EP_RX_VALID);
}

/*******************************************************************************
* Function Name  : EP2_OUT_Callback.
* Description    : EP2 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
	static  dev_config_t tmp_dev_config;
	
	uint8_t config_in_cnt;
	uint8_t config_out_cnt;
	uint8_t tmp_buf[64];
	uint8_t hid_buf[64];
	uint8_t repotId;

	/* Read received data (2 bytes) */
  USB_SIL_Read(EP2_OUT, hid_buf);
	
	repotId = hid_buf[0];
	
	if (repotId == REPORT_ID_PARAM)
	{
		configurator_millis = GetMillis() + 30000;
		SetEPRxStatus(ENDP2, EP_RX_VALID);
		return;
	}
	else 
	{
		// 2 second delay for joy report
		joy_millis = GetMillis() + 2000;
		adc_ticks = (GetMillis() + 2000) * TICKS_IN_MILLISECOND;
		buttons_ticks = (GetMillis() + 2000) * TICKS_IN_MILLISECOND;
		sensors_ticks = (GetMillis() + 2000) * TICKS_IN_MILLISECOND;
		encoder_ticks = (GetMillis() + 2000) * TICKS_IN_MILLISECOND;
	}
	
	uint8_t cfg_count = sizeof(dev_config_t) / 62;
  uint8_t last_cfg_size = sizeof(dev_config_t) % 62;
	if (last_cfg_size > 0)
	{
		cfg_count++;
	}
	
	switch (repotId)
	{
		case REPORT_ID_CONFIG_IN:
		{
			config_in_cnt = hid_buf[1];			// requested config packet number
			
			if ((config_in_cnt > 0) & (config_in_cnt <= cfg_count))
			{		
				DevConfigGet(&tmp_dev_config);
				
				memset(tmp_buf, 0, sizeof(tmp_buf));
				tmp_buf[0] = REPORT_ID_CONFIG_IN;					
				tmp_buf[1] = config_in_cnt;
				
				if (config_in_cnt == cfg_count && last_cfg_size > 0)
				{
					memcpy(&tmp_buf[2], (uint8_t *) &(tmp_dev_config) + 62*(config_in_cnt-1), last_cfg_size);
				}
				else
				{
					memcpy(&tmp_buf[2], (uint8_t *) &(tmp_dev_config) + 62*(config_in_cnt-1), 62);
				}
				
				USB_CUSTOM_HID_SendReport(2, (uint8_t *)&(tmp_buf), 64);							
			}
		}
		break;
		
		case REPORT_ID_CONFIG_OUT:
		{
			if (hid_buf[1] == cfg_count && last_cfg_size > 0)
			{
				memcpy((uint8_t *) &(tmp_dev_config) + 62*(hid_buf[1]-1), &hid_buf[2], last_cfg_size);
			}
			else if (hid_buf[1] > 0)
			{
				memcpy((uint8_t *) &(tmp_dev_config) + 62*(hid_buf[1]-1), &hid_buf[2], 62);
			}
			
			if (hid_buf[1] < cfg_count)		// request new packet
			{
				config_out_cnt = hid_buf[1] + 1;
				
				uint8_t tmp_buf[2];
				tmp_buf[0] = REPORT_ID_CONFIG_OUT;
				tmp_buf[1] = config_out_cnt;
				
				USB_CUSTOM_HID_SendReport(2, tmp_buf, 2);
			}
			else // last packet received
			{
				// Check if config version matches
				if ((tmp_dev_config.firmware_version &0xFFF0) != (FIRMWARE_VERSION & 0xFFF0))
				{
					// Report error
					uint8_t tmp_buf[2];
					tmp_buf[0] = REPORT_ID_CONFIG_OUT;
					tmp_buf[1] = 0xFE;
					USB_CUSTOM_HID_SendReport(2, tmp_buf, 2);
					
					// blink LED if firmware version doesnt match
					GPIO_InitTypeDef GPIO_InitStructure;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
					GPIO_Init(GPIOC, &GPIO_InitStructure);
					
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
					GPIO_Init(GPIOB, &GPIO_InitStructure);
					
					for (uint8_t i=0; i<6; i++) 
					{
						
						GPIOB->ODR ^= GPIO_Pin_12;
						GPIOC->ODR ^=	GPIO_Pin_13;
						Delay_us(200000);
					}
				}
				else
				{
					tmp_dev_config.firmware_version = FIRMWARE_VERSION;
					DevConfigSet(&tmp_dev_config);
					NVIC_SystemReset();
				}		
			}
		}
		break;
			
		case REPORT_ID_FIRMWARE:
		{
			const char tmp_str[] = "bootloader run";

			if (strcmp(tmp_str, (const char *) &hid_buf[1]) == 0)
			{
				bootloader = 1;
			}
		}
		break;
		
		default:
			break;
	}

	memset(hid_buf, 0 ,64);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
 
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  EP1_PrevXferComplete = 1;
}

/*******************************************************************************
* Function Name  : EP2_IN_Callback.
* Description    : EP2 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_IN_Callback(void)
{
  EP2_PrevXferComplete = 1;
}

/*******************************************************************************
* Function Name  : USB_CUSTOM_HID_SendReport.
* Description    : 
* Input          : None.
* Output         : None.
* Return         : 1 if success otherwise 0.
*******************************************************************************/
int8_t USB_CUSTOM_HID_SendReport(uint8_t EP_num, uint8_t * data, uint8_t length)
{
	if ((EP_num == 1) && (EP1_PrevXferComplete) && (bDeviceState == CONFIGURED))
	{
			USB_SIL_Write(EP1_IN, data, length);
			SetEPTxValid(ENDP1);
			EP1_PrevXferComplete = 0;
			return 0;
	}
	else if ((EP_num == 2) && (EP2_PrevXferComplete) && (bDeviceState == CONFIGURED))
	{
			USB_SIL_Write(EP2_IN, data, length);
			SetEPTxValid(ENDP2);
			EP2_PrevXferComplete = 0;
			return 0;
	}
	return -1;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

