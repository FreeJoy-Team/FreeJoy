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

#include "flash.h"
#include "crc16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile extern uint8_t bootloader;
volatile extern int32_t joy_millis;

__IO uint8_t PrevXferComplete = 1;

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
	static  app_config_t tmp_config;
	static  uint16_t firmware_len = 0;
	
	uint8_t config_in_cnt;
	uint8_t config_out_cnt;
	uint8_t tmp_buf[64];
	uint8_t hid_buf[64];
	uint8_t i;
	uint8_t pos = 2;
	uint8_t repotId;

	// 2 second delay for joy report
	joy_millis = GetTick() + 2000;
	Timers_Pause(2000);
	
	/* Read received data (2 bytes) */  
  USB_SIL_Read(EP1_OUT, hid_buf);
	
	repotId = hid_buf[0];
	
	switch (repotId)
	{
		case REPORT_ID_CONFIG_IN:
		{
			config_in_cnt = hid_buf[1];			// requested config packet number
			
			if ((config_in_cnt > 0) & (config_in_cnt <= 12))
			{		
				
				uint8_t pos = 2;
				uint8_t i;
				
				ConfigGet(&tmp_config);
				
				memset(tmp_buf, 0, sizeof(tmp_buf));			
				tmp_buf[0] = REPORT_ID_CONFIG_IN;					
				tmp_buf[1] = config_in_cnt;
				
				switch(config_in_cnt)
				{
						case 1:	
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.firmware_version), sizeof(tmp_config.firmware_version));
							pos += sizeof(tmp_config.firmware_version);
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.device_name), sizeof(tmp_config.device_name));
							pos += sizeof(tmp_config.device_name);
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.button_debounce_ms), 8);
							pos += 8;
							
							memcpy(&tmp_buf[63-sizeof(tmp_config.pins)], (uint8_t *) &(tmp_config.pins), sizeof(tmp_config.pins));
						break;
					
					case 2:
						i = 0;
						while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
						{
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axis_config[i++]), sizeof(axis_config_t));
							pos += sizeof(axis_config_t);
						}
						break;
					
					case 3:
						i = 2;
						while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
						{
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axis_config[i++]), sizeof(axis_config_t));
							pos += sizeof(axis_config_t);
						}
						break;
					
					case 4:
						i = 4;
						while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
						{
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axis_config[i++]), sizeof(axis_config_t));
							pos += sizeof(axis_config_t);
						}
						break;

					case 5:

						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axis_config[6]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axis_config[7]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
						
						break;
					
					case 6:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.buttons[0]), 62);
						break;
					
					case 7:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.buttons[31]), 62);
						break;
					
					case 8:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.buttons[62]), 62);
						break;
					
					case 9:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.buttons[93]), 62);
						break;
					
					case 10:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.buttons[124]), 4*sizeof(button_t));
						pos += 4*sizeof(button_t);
					
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[0]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[1]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);					
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[2]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);		
						break;
					
					case 11:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[3]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[4]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);					
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[5]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);	
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[6]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);	
						break;
					
					case 12:
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.axes_to_buttons[7]), sizeof(axis_to_buttons_t));	
						pos += sizeof(axis_to_buttons_t);	
					
						for (i=0; i<4; i++)
						{
							memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_registers[i]), sizeof(shift_reg_config_t));
							pos += sizeof(shift_reg_config_t);
						}
						
						
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_config[0]), sizeof(shift_modificator_t));
						pos += sizeof(shift_modificator_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_config[1]), sizeof(shift_modificator_t));
						pos += sizeof(shift_modificator_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_config[2]), sizeof(shift_modificator_t));
						pos += sizeof(shift_modificator_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_config[3]), sizeof(shift_modificator_t));
						pos += sizeof(shift_modificator_t);
						memcpy(&tmp_buf[pos], (uint8_t *) &(tmp_config.shift_config[4]), sizeof(shift_modificator_t));
						pos += sizeof(shift_modificator_t);
						break;
						
					default:
						break;
						
					
				}
					
				USB_CUSTOM_HID_SendReport((uint8_t *)&(tmp_buf), 64);
				config_in_cnt = 0;	
			}
		}
		break;
		
		case REPORT_ID_CONFIG_OUT:
		{
			switch (hid_buf[1])
			{
				case 1:
					{
						memcpy((uint8_t *) &(tmp_config.firmware_version), &hid_buf[pos], sizeof(tmp_config.firmware_version));
						pos += sizeof(tmp_config.firmware_version);
						memcpy((uint8_t *) &(tmp_config.device_name), &hid_buf[pos], sizeof(tmp_config.device_name));
						pos += sizeof(tmp_config.device_name);
						memcpy((uint8_t *) &(tmp_config.button_debounce_ms), &hid_buf[pos], 8);
						pos += 8;
						
						memcpy((uint8_t *) &(tmp_config.pins), &hid_buf[63-sizeof(tmp_config.pins)], sizeof(tmp_config.pins));
					}
				break;
				
				case 2:
				{
					i = 0;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;
				
				case 3:
				{
					i = 2;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;
				
				case 4:
				{
					i = 4;
					while(64 - pos > sizeof(axis_config_t))
					{
						memcpy((uint8_t *) &(tmp_config.axis_config[i++]), &hid_buf[pos], sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
				}
				break;

				case 5:
				{
					memcpy((uint8_t *) &(tmp_config.axis_config[6]), &hid_buf[pos], sizeof(axis_config_t));
					pos += sizeof(axis_config_t);
					memcpy((uint8_t *) &(tmp_config.axis_config[7]), &hid_buf[pos], sizeof(axis_config_t));
					pos += sizeof(axis_config_t);
					
				}
				break;
				
				case 6:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[0]), &hid_buf[pos], 62);
				}
				break;
				
				case 7:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[31]), &hid_buf[pos], 62);
				}
				break;
				
				case 8:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[62]), &hid_buf[pos], 62);
				}
				break;
				
				case 9:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[93]), &hid_buf[pos], 62);
				}
				break;
				
				case 10:
				{
					memcpy((uint8_t *) &(tmp_config.buttons[124]), &hid_buf[pos], 4*sizeof(button_t));
					pos += 4*sizeof(button_t);
					
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[0]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[1]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[2]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
				}
				break;
				
				case 11:
				{
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[3]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[4]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[5]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[6]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					break;
				}
				case 12:
				{
					memcpy((uint8_t *) &(tmp_config.axes_to_buttons[7]), &hid_buf[pos], sizeof(axis_to_buttons_t));
					pos += sizeof(axis_to_buttons_t);
					
					for (i=0; i<4; i++)
					{
						memcpy((uint8_t *) &(tmp_config.shift_registers[i]), &hid_buf[pos], sizeof(shift_reg_config_t));
						pos += sizeof(shift_reg_config_t);
					}
					
					
					memcpy((uint8_t *) &(tmp_config.shift_config[0]), &hid_buf[pos], sizeof(shift_modificator_t));
					pos += sizeof(shift_modificator_t);
					memcpy((uint8_t *) &(tmp_config.shift_config[1]), &hid_buf[pos], sizeof(shift_modificator_t));
					pos += sizeof(shift_modificator_t);
					memcpy((uint8_t *) &(tmp_config.shift_config[2]), &hid_buf[pos], sizeof(shift_modificator_t));
					pos += sizeof(shift_modificator_t);
					memcpy((uint8_t *) &(tmp_config.shift_config[3]), &hid_buf[pos], sizeof(shift_modificator_t));
					pos += sizeof(shift_modificator_t);
					memcpy((uint8_t *) &(tmp_config.shift_config[4]), &hid_buf[pos], sizeof(shift_modificator_t));
					pos += sizeof(shift_modificator_t);
				}					
					break;
				
				default:
					break;
			}
			if (hid_buf[1] < 12)		// request new packet
			{
				config_out_cnt = hid_buf[1] + 1;
				
				uint8_t tmp_buf[2];
				tmp_buf[0] = REPORT_ID_CONFIG_OUT;
				tmp_buf[1] = config_out_cnt;
				
				USB_CUSTOM_HID_SendReport(tmp_buf,2);
			}
			else // last packet received
			{
				if ((tmp_config.firmware_version &0xFFF0) != (FIRMWARE_VERSION & 0xFFF0))
				{
					GPIO_InitTypeDef GPIO_InitStructure;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
					GPIO_Init(GPIOC, &GPIO_InitStructure);
					
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
					GPIO_Init(GPIOB, &GPIO_InitStructure);
					
					for (uint8_t i=0; i<6; i++) 
					{
						// blink LED if firmware version doesnt match
						GPIOB->ODR ^= GPIO_Pin_12;
						GPIOC->ODR ^=	GPIO_Pin_13;
						Delay_ms(200);
					}
				}
				else
				{
					tmp_config.firmware_version = FIRMWARE_VERSION;
					ConfigSet(&tmp_config);
				}
				
				NVIC_SystemReset();
			}
		}
		break;
			
		case REPORT_ID_FIRMWARE:
		{
			uint16_t crc_in = 0;
			uint16_t crc_comp = 0;
			uint16_t firmware_in_cnt = 0;
			
			uint16_t cnt = hid_buf[1]<<8 | hid_buf[2];
			
			if (cnt == 0)			// first packet with info data
			{
				firmware_len = hid_buf[5]<<8 | hid_buf[4];
				
				if (firmware_len <= 0x7000)	// check new firmware size
				{
					FLASH_Unlock();
					for (uint8_t i=0; i<28; i++)
					{
						FLASH_ErasePage(FIRMWARE_COPY_ADDR);
					
					}
					for (uint8_t i=0;i<60;i+=2)
					{
						uint16_t tmp16 = hid_buf[i + 5]<<8 | hid_buf[i + 4];
						FLASH_ProgramHalfWord(FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
					}
					FLASH_Lock();
					firmware_in_cnt = cnt+1;
				}
				else // firmware size error
				{
					
				}
			}
			else if ( (firmware_len > 0) && (cnt*60 < firmware_len) )		// body of firmware data
			{
				FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hid_buf[i + 5]<<8 | hid_buf[i + 4];
					FLASH_ProgramHalfWord(FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
				}
				FLASH_Lock();
				firmware_in_cnt = cnt+1;
			}
			else if (firmware_len > 0)		// last packet
			{
				FLASH_Unlock();
				for (uint8_t i=0;i<60;i+=2)
				{
					uint16_t tmp16 = hid_buf[i + 5]<<8 | hid_buf[i + 4];
					FLASH_ProgramHalfWord(FIRMWARE_COPY_ADDR + (cnt)*60 + i, tmp16);
				}
				FLASH_Lock();
				
				// check CRC16
				crc_in = (*(uint8_t *) (FIRMWARE_COPY_ADDR + 3)) << 8 | (*(uint8_t *) (FIRMWARE_COPY_ADDR + 2));				
				crc_comp = Crc16((uint8_t*)FIRMWARE_COPY_ADDR + 60, firmware_len);				
				if (crc_in == crc_comp && crc_comp != 0)
				{
					bootloader = 1;
				}
			}
			else break;
			
			if (firmware_in_cnt > 0)
			{
				uint8_t tmp_buf[3];
				tmp_buf[0] = REPORT_ID_FIRMWARE;
				tmp_buf[1] = (firmware_in_cnt)>>8;
				tmp_buf[2] = (firmware_in_cnt)&0xFF;
				
				USB_CUSTOM_HID_SendReport(tmp_buf,3);

			}
			
		}
		break;
		
		default:
			break;
	}

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
  PrevXferComplete = 1;
}

void USB_CUSTOM_HID_SendReport(uint8_t * data, uint8_t length)
{
	if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
	{
			USB_SIL_Write(EP1_IN, data, length);
			SetEPTxValid(ENDP1);
			PrevXferComplete = 0;
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

