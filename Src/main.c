
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
	
		FreeJoy software for game device controllers
    Copyright (C) 2020  Yury Vostrenkov (yuvostrenkov@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
		
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "periphery.h"
#include "config.h"
#include "analog.h"
#include "buttons.h"
#include "encoders.h"

#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_pwr.h"


/* Private variables ---------------------------------------------------------*/
dev_config_t dev_config;
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
	DevConfigGet(&dev_config);
	
	// set default config at first startup
	if ((dev_config.firmware_version & 0xFFF0) != (FIRMWARE_VERSION &0xFFF0))
	{
		DevConfigSet((dev_config_t *) &init_config);
		DevConfigGet(&dev_config);
	}
	AppConfigInit(&dev_config);
	
	Delay_ms(50);
	
	USB_HW_Init(&dev_config);
	IO_Init(&dev_config);
	AxesInit(&dev_config); 
	EncodersInit(&dev_config);	
	ShiftRegistersInit(&dev_config);
	RadioButtons_Init(&dev_config);
	
	Timers_Init();

  while (1)
  {
		ButtonsReadLogical(&dev_config);
		// jump to bootloader if new firmware received
		if (bootloader > 0)
		{
			Delay_ms(50);	// time to let HID end last transmission
			// Disable USB
			PowerOff();
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

