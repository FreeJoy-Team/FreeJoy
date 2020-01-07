
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_customhid.h"
#include "usbd_desc.h"
#include "periphery.h"
#include "flash.h"
#include "analog.h"
#include "buttons.h"

#define REPORT_LEN_16			MAX_AXIS_NUM + MAX_BUTTONS_NUM/16
#define REPORT_LEN_8			REPORT_LEN_16 * 2

/* Private variables ---------------------------------------------------------*/
app_config_t config;
volatile uint8_t config_in_cnt;
volatile uint8_t config_out_cnt;
volatile uint16_t firmware_in_cnt;
volatile uint8_t bootloader = 0;
joy_report_t joy_report;
uint8_t tmp_buf[64];
uint32_t uid[3];

//uint8_t report_data[64];

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
	int32_t millis =0, joy_millis=0;
	
  HAL_Init();
	
	SystemClock_Config();
	
	ConfigGet(&config);
	// set default config at first startup
	if (config.firmware_version == 0xFFFF)
	{
		ConfigSet((app_config_t *) &init_config);
		ConfigGet(&config);
	}
	// set HID name and S/N
	memcpy(USBD_PRODUCT_STRING_FS, config.device_name, sizeof(USBD_PRODUCT_STRING_FS));
	
	HAL_GetUID(uid);
	uid[0] = uid[0] ^ uid[1] ^ uid[2]; 
	sprintf(USBD_SERIALNUMBER_STRING_FS, "%012X", uid[0]); 
	
	MX_USB_DEVICE_Init();

	GPIO_Init(&config);
	ADC_Init(&config); 
	EncodersInit(&config);	

  while (1)
  {
		millis = HAL_GetTick();
		
		if ((config_in_cnt > 0) & (config_in_cnt <= 10))
		{		
			
			uint8_t pos = 2;
			uint8_t i;
			
			memset(tmp_buf, 0, sizeof(tmp_buf));			
			tmp_buf[0] = REPORT_ID_CONFIG_IN;					
			tmp_buf[1] = config_in_cnt;
			
			switch(config_in_cnt)
			{
					case 1:	
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.firmware_version), sizeof(config.firmware_version));
						pos += sizeof(config.firmware_version);
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.device_name), sizeof(config.device_name));
						pos += sizeof(config.device_name);
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.button_debounce_ms), 8);
						pos += 8;
						
						memcpy(&tmp_buf[63-sizeof(config.pins)], (uint8_t *) &(config.pins), sizeof(config.pins));
					break;
				
				case 2:
					i = 0;
					while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axis_config[i++]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
					break;
				
				case 3:
					i = 2;
					while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axis_config[i++]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
					break;
				
				case 4:
					i = 4;
					while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axis_config[i++]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
					break;

				case 5:
					i = 6;
					while(sizeof(tmp_buf) - pos > sizeof(axis_config_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axis_config[i++]), sizeof(axis_config_t));
						pos += sizeof(axis_config_t);
					}
					break;
				
				case 6:
					memcpy(&tmp_buf[pos], (uint8_t *) &(config.buttons[0]), 62);
					break;
				
				case 7:
					memcpy(&tmp_buf[pos], (uint8_t *) &(config.buttons[62]), 62);
					break;
				
				case 8:
					memcpy(&tmp_buf[pos], (uint8_t *) &(config.buttons[124]), 4);
					pos += 4;
				
					i = 0;
					while(sizeof(tmp_buf) - pos > sizeof(axis_to_buttons_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axes_to_buttons[i++]), sizeof(axis_to_buttons_t));
						pos += sizeof(axis_to_buttons_t);
					}
					break;
				
				case 9:
					i = 4;
					while(sizeof(tmp_buf) - pos > sizeof(axis_to_buttons_t))
					{
						memcpy(&tmp_buf[pos], (uint8_t *) &(config.axes_to_buttons[i++]), sizeof(axis_to_buttons_t));
						pos += sizeof(axis_to_buttons_t);
					}
					break;
				
				case 10:
					
					break;
					
				default:
					break;
				
			}
				
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(tmp_buf), 64);
			config_in_cnt = 0;	
			// 1 second delay for joy report in config mode
			joy_millis = millis + 1000;
		}
		
		if ((config_out_cnt > 1) & (config_out_cnt <= 10))
		{	
			uint8_t tmp_buf[2];
			tmp_buf[0] = REPORT_ID_CONFIG_OUT;
			tmp_buf[1] = config_out_cnt;
			
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(tmp_buf), 2);
			config_out_cnt = 0;	
			// 1 second delay for joy report in config mode
			joy_millis = millis + 1000;
		}
		
		if (firmware_in_cnt > 0)
		{
			uint8_t tmp_buf[3];
			tmp_buf[0] = REPORT_ID_FIRMWARE;
			tmp_buf[1] = (firmware_in_cnt)>>8;
			tmp_buf[2] = (firmware_in_cnt)&0xFF;
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(tmp_buf), 3);
			
			firmware_in_cnt = 0;
			// 1 second delay for joy report in config mode
			joy_millis = millis + 1000;
		}
		
		if (millis - joy_millis > config.exchange_period_ms)
		{
			joy_millis = millis;
			
			joy_report.id = REPORT_ID_JOY;
			
			
			
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(joy_report.id), sizeof(joy_report)-sizeof(joy_report.dummy));
		}
		
		// jump to bootloader
		if (bootloader > 0)
		{
			EnterBootloader();
		}
		
		ButtonsGet(joy_report.button_data);
		AnalogGet(joy_report.axis_data, joy_report.raw_axis_data);	
		POVsGet(joy_report.pov_data);
  }
}

void EnterBootloader (void)
{
	uint32_t bootloader_addr;
	typedef void(*pFunction)(void);
	pFunction Bootloader;
	
	bootloader = 0;
	
	__disable_irq();
	bootloader_addr = *(uint32_t*) (BOOTLOADER_ADDR + 4);
	
	Bootloader = (pFunction) bootloader_addr;
	
	__set_MSP(*(__IO uint32_t*) BOOTLOADER_ADDR);
	
	Bootloader();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

