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

#include "periphery.h"
#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



#define REPORT_ID_FIRMWARE				0x04


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__IO uint8_t EP1_PrevXferComplete = 1;
static volatile uint16_t crc_in = 0;

volatile bool flash_started = 0;
volatile bool flash_finished = 0;

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
	static  uint16_t firmware_len = 0;

	uint8_t hid_buf[64];
	uint8_t repotId;
	
	/* Read received data (2 bytes) */  
  USB_SIL_Read(EP1_OUT, hid_buf);
	
	repotId = hid_buf[0];
	
	LED1_ON;
	switch (repotId)
	{		
		case REPORT_ID_FIRMWARE:
		{			
			uint16_t crc_comp = 0;
			uint16_t firmware_in_cnt = 0;
			
			uint16_t cnt = hid_buf[1]<<8 | hid_buf[2];
			
			if (cnt == 0)			// first packet with info data
			{
				firmware_len = hid_buf[5]<<8 | hid_buf[4];
				crc_in = hid_buf[7]<<8 | hid_buf[6];
				
				if (firmware_len <= 0xE000)	// check new firmware size, 56kB max
				{
					flash_started = 1;
					
					FLASH_Unlock();
					for (uint8_t i=0; i<MAX_PAGE-FIRMWARE_START_PAGE; i++)
					{
						if (FLASH_ErasePage(FIRMWARE_COPY_ADDR +i*FLASH_PAGE_SIZE) != FLASH_COMPLETE)
						{
							firmware_in_cnt = 0xF003;	// flash erase error
						}							
					}
					FLASH_Lock();
					firmware_in_cnt = cnt+1;
				}
				else // firmware size error
				{
					firmware_in_cnt = 0xF001;
				}
			}
			else if (flash_started && (firmware_len > 0) && (cnt*60 < firmware_len) )		// body of firmware data
			{
				FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hid_buf[i + 5]<<8 | hid_buf[i + 4];
					FLASH_ProgramHalfWord(FIRMWARE_COPY_ADDR + (cnt-1)*60 + i, tmp16);
				}
				FLASH_Lock();
				firmware_in_cnt = cnt+1;
			}
			else if (flash_started && firmware_len > 0)		// last packet
			{
				FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hid_buf[i + 5]<<8 | hid_buf[i + 4];
					FLASH_ProgramHalfWord(FIRMWARE_COPY_ADDR + (cnt-1)*60 + i, tmp16);
				}
				FLASH_Lock();
				
				// check CRC16				
				crc_comp = Crc16((uint8_t*)FIRMWARE_COPY_ADDR, firmware_len);				
				if (crc_in == crc_comp && crc_comp != 0)
				{
					flash_started = 0;
					flash_finished = 1;
					firmware_in_cnt = 0xF000;	// OK
				}
				else	// CRC error
				{
					firmware_in_cnt = 0xF002;
				}
			}
			
			if (firmware_in_cnt > 0)
			{
				uint8_t tmp_buf[3];
				tmp_buf[0] = REPORT_ID_FIRMWARE;
				tmp_buf[1] = (firmware_in_cnt)>>8;
				tmp_buf[2] = (firmware_in_cnt)&0xFF;
				
				USB_CUSTOM_HID_SendReport(1, tmp_buf, 3);

			}
			
		}
		break;
		
		default:
			break;
	}
	
	LED1_OFF;	
  SetEPRxStatus(ENDP1, EP_RX_VALID);
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

int8_t USB_CUSTOM_HID_SendReport(uint8_t EP_num, uint8_t * data, uint8_t length)
{
	if ((EP1_PrevXferComplete) && (bDeviceState == CONFIGURED))
	{
			USB_SIL_Write(EP1_IN, data, length);
			SetEPTxValid(ENDP1);
			EP1_PrevXferComplete = 0;
			return 0;
	}
	return -1;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

