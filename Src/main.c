
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
volatile uint8_t adc_data_ready = 0;
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
	uint32_t millis, prev_millis;
	
  HAL_Init();
	
	SystemClock_Config();
	
	MX_USB_DEVICE_Init();
	
	ConfigSet((app_config_t *) &init_config);
	ConfigGet(&config);

	GPIO_Init(&config);
	ADC_Init(&config);
	
  

  while (1)
  {
		millis = HAL_GetTick();
		
		if (millis - prev_millis > 10)
		{
			prev_millis = millis;
			
			joy_report.id = JOY_REPORT_ID;
			
			ButtonsGet(joy_report.button_data);
			AnalogGet(joy_report.axis_data);	
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

