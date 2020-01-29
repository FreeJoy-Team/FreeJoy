
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
uint8_t btn_num = 0;
uint8_t	physical_buttons_data[MAX_BUTTONS_NUM];

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
	SysTick_Init();
	// getting configuration from flash memory
	ConfigGet(&config);
	
	// set default config at first startup
	if (config.firmware_version != FIRMWARE_VERSION)
	{
		ConfigSet((app_config_t *) &init_config);
		ConfigGet(&config);
	}
	
	Delay_ms(50);
	
	USB_HW_Init();
	IO_Init(&config);
	AxesInit(&config); 
	EncodersInit(&config);	
	ShiftRegistersInit(&config);

  while (1)
  {
		millis = GetTick();

		// check if it is time to send joystick data
		if (millis - joy_millis > config.exchange_period_ms )
		{
			joy_millis = millis;
			
			joy_report.id = REPORT_ID_JOY;		
			joy_report.raw_button_data[0] = btn_num;
			for (uint8_t i=0; i<8; i++)	joy_report.raw_button_data[1+i] = physical_buttons_data[btn_num+i];
			btn_num += 8;
			btn_num = btn_num & 0x7F;
			
			
			USB_CUSTOM_HID_SendReport((uint8_t *)&(joy_report.id), sizeof(joy_report)-sizeof(joy_report.dummy));
		}
		
		// jump to bootloader if new firmware received
		if (bootloader > 0)
		{
			EnterBootloader();
		}
		
		
		// check if it is time to update data
		if (millis > last_millis)
		{
			last_millis = millis;
			
			
			// buttons routine
			ButtonsCheck(&config);
			
			// axes routine
			AxesProcess(&config);
			
			// getting fresh data to joystick report buffer
			ButtonsGet(physical_buttons_data, joy_report.button_data);
			AnalogGet(joy_report.axis_data, NULL, joy_report.raw_axis_data);	
			POVsGet(joy_report.pov_data);
			
			
		}
  }
}

/**
  * @brief  Jumping to memory address corresponding bootloader program
  * @param  None
  * @retval None
  */
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

