
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "periphery.h"
#include "flash.h"
#include "analog.h"
#include "buttons.h"

#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_pwr.h"


/* Private variables ---------------------------------------------------------*/
app_config_t config;
volatile uint8_t bootloader = 0;
joy_report_t joy_report;
volatile int32_t millis =0, last_millis=0, joy_millis=0;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{

	SysTick_Init();
	
	ConfigGet(&config);
	// set default config at first startup
	if (config.firmware_version != FIRMWARE_VERSION)
	{
		ConfigSet((app_config_t *) &init_config);
		ConfigGet(&config);
	}
	
	// Init USB
	USB_HW_Init();

	IO_Init(&config);
	AxesInit(&config); 
	EncodersInit(&config);	
	ShiftRegistersInit(&config);

  while (1)
  {
		millis = GetTick();

		if (millis - joy_millis > config.exchange_period_ms )
		{
			joy_millis = millis;
			
			joy_report.id = REPORT_ID_JOY;			
			USB_CUSTOM_HID_SendReport((uint8_t *)&(joy_report.id), sizeof(joy_report)-sizeof(joy_report.dummy));
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
			AnalogGet(joy_report.axis_data, NULL, joy_report.raw_axis_data);	
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
  * @}
  */

/**
  * @}
  */

