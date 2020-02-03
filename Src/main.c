
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
#include "encoders.h"

#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_pwr.h"


/* Private variables ---------------------------------------------------------*/
app_config_t config;
volatile uint8_t bootloader = 0;


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
	
	Timers_Init();

  while (1)
  {
		
		ButtonsReadLogical(&config);
		// jump to bootloader if new firmware received
		if (bootloader > 0)
		{
			EnterBootloader();
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

