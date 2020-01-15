
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

#define REPORT_LEN_16			(MAX_AXIS_NUM + MAX_BUTTONS_NUM/16)
#define REPORT_LEN_8			(REPORT_LEN_16 * 2)

/* Private variables ---------------------------------------------------------*/
app_config_t config;
volatile uint8_t bootloader = 0;
joy_report_t joy_report;
volatile int32_t millis =0, last_millis=0, joy_millis=0;

//uint8_t report_data[64];

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
	uint32_t uid[3];
	
  HAL_Init();
	
	SystemClock_Config();
	
	ConfigGet(&config);
	// set default config at first startup
	if (config.firmware_version != FIRMWARE_VERSION)
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
	AxesInit(&config); 
	EncodersInit(&config);	
	ShiftRegistersInit(&config);

  while (1)
  {
		millis = HAL_GetTick();

		if (millis - joy_millis > config.exchange_period_ms )
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
		
		if (millis > last_millis)
		{
			last_millis = millis;
			
			ButtonsCheck(&config);
			AxesProcess(&config);
			
			ButtonsGet(joy_report.button_data);
			AnalogGet(joy_report.axis_data, joy_report.raw_axis_data);	
			POVsGet(joy_report.pov_data);
		}
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

