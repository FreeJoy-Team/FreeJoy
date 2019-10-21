
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
#include "periphery.h"
#include "flash.h"
#include "analog.h"
#include "buttons.h"

#define REPORT_LEN_16			MAX_AXIS_NUM + MAX_BUTTONS_NUM/16
#define REPORT_LEN_8			REPORT_LEN_16 * 2

/* Private variables ---------------------------------------------------------*/
app_config_t config;
volatile uint8_t config_requested;
volatile uint8_t config_requesting;
joy_report_t joy_report;

//uint8_t report_data[64];

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
	int32_t millis =0, joy_millis=0, config_millis=0;
	
  HAL_Init();
	
	SystemClock_Config();
	
	MX_USB_DEVICE_Init();
	
	//ConfigSet((app_config_t *) &init_config);
	ConfigGet(&config);

	GPIO_Init(&config);
	ADC_Init(&config);  

  while (1)
  {
		millis = HAL_GetTick();
		
		if ((config_requested > 0) & (config_requested <= 10) & (millis > config_millis))
		{		
			uint8_t tmp_buf[64];
			uint8_t pos = 2;
			uint8_t i;
			
			memset(tmp_buf, 0, sizeof(tmp_buf));			
			tmp_buf[0] = CONFIG_IN_REPORT_ID;					
			tmp_buf[1] = config_requested;
			
			switch(config_requested)
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
				
					memcpy(&tmp_buf[63-sizeof(config.encoders)], (uint8_t *) &(config.encoders), sizeof(config.encoders));
					break;
				
				case 9:
					
					break;
				
				case 10:
					
					break;
					
				default:
					break;
				
			}
				
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(tmp_buf), 64);
			config_requested = 0;	
			config_millis = millis;
			// 1 second delay for joy report in config mode
			joy_millis = millis + 1000;
		}
		
		if ((config_requesting > 1) & (config_requesting <= 10) & (millis > config_millis))
		{	
			uint8_t tmp_buf[2];
			tmp_buf[0] = CONFIG_OUT_REPORT_ID;
			tmp_buf[1] = config_requesting;
			
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(tmp_buf), 2);
			config_requesting = 0;	
			config_millis = millis;
			// 1 second delay for joy report in config mode
			joy_millis = millis + 1000;
		}
		
		if (millis - joy_millis > config.exchange_period_ms)
		{
			joy_millis = millis;
			
			joy_report.id = JOY_REPORT_ID;
			
			ButtonsGet(joy_report.button_data);
			AnalogGet(joy_report.axis_data, joy_report.raw_axis_data);	
			POVsGet(joy_report.pov_data);
			
			USBD_CUSTOM_HID_SendReport(	&hUsbDeviceFS, (uint8_t *)&(joy_report.id), sizeof(joy_report)-sizeof(joy_report.dummy));
		}
  }
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

