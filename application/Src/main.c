
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
#include "leds.h"
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
	// Relocate vector table
	WRITE_REG(SCB->VTOR, 0x8002000);
	
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
	
	IO_Init(&dev_config);
	 
	EncodersInit(&dev_config);	
	ShiftRegistersInit(&dev_config);
	RadioButtons_Init(&dev_config);
	SequentialButtons_Init(&dev_config);
	AxesInit(&dev_config);
	
	Delay_ms(50);	
	USB_HW_Init();
	
	Timers_Init(&dev_config);		
	
  while (1)
  {
		ButtonsReadLogical(&dev_config);
		LEDs_PhysicalProcess(&dev_config);
		
		analog_data_t tmp[8];
		AnalogGet(NULL, tmp, NULL);
		PWM_SetFromAxes(&dev_config, tmp);
		
		// Enter flasher command received
		if (bootloader > 0)
		{
			// Disable HID report generation
			NVIC_DisableIRQ(TIM2_IRQn);
			Delay_ms(50);	// time to let HID end last transmission
			// Disable USB
			PowerOff();
			USB_HW_DeInit();
			Delay_ms(500);	
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
	/* Enable the power and backup interface clocks by setting the
	 * PWREN and BKPEN bits in the RCC_APB1ENR register
	 */
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN);

	/* Enable write access to the backup registers and the
		* RTC.
		*/
	SET_BIT(PWR->CR, PWR_CR_DBP);
	WRITE_REG(BKP->DR4, 0x424C);
	CLEAR_BIT(PWR->CR, PWR_CR_DBP);
	
	CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN);
	
	NVIC_SystemReset();
}


/**
  * @}
  */

/**
  * @}
  */

