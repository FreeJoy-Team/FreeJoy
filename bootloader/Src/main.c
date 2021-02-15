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

#include "periphery.h"
#include "usb_hw.h"

/* Private types */
typedef void (*funct_ptr)(void);
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static bool CheckUserCode(uint32_t user_address);

static uint16_t GetMagicWord(void);

static void EnterProgram(void);

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void) 
{
    IO_Init();
    Delay(100);

    uint16_t magic_word = GetMagicWord();
    uint16_t boot1 = READ_BIT(GPIOB->IDR, GPIO_IDR_IDR2);
    uint16_t checkUserCode = CheckUserCode(FIRMWARE_COPY_ADDR);

    if ((magic_word == 0x424C) || boot1 || checkUserCode == 0) 
		{
        USB_HW_Init();
    } 
		else 
		{
        EnterProgram();
        // Never reached
        while (1);
    }

    while (1) 
		{
        if (!flash_started) 
				{
            LED1_ON;
            Delay(500000);
            LED1_OFF;
            Delay(10000000);
        }
        if (flash_finished) 
				{
            Delay(100000);
            USB_Shutdown();
            Delay(1000000);
            EnterProgram();
        }
    }
}

static bool CheckUserCode(uint32_t user_address) 
{
    uint32_t sp = *(volatile uint32_t *) user_address;

    /* Check if the stack pointer in the vector table points
       somewhere in SRAM */
    return ((sp & 0x2FFE0000) == SRAM_BASE) ? 1 : 0;
}

static uint16_t GetMagicWord(void) 
{
    /* Enable the power and backup interface clocks by setting the
     * PWREN and BKPEN bits in the RCC_APB1ENR register
     */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN);
    uint16_t value = READ_REG(BKP->DR4);
    if (value) 
		{

        /* Enable write access to the backup registers and the
         * RTC.
         */
        SET_BIT(PWR->CR, PWR_CR_DBP);
        WRITE_REG(BKP->DR4, 0x0000);
        CLEAR_BIT(PWR->CR, PWR_CR_DBP);
    }
    CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN);

    return value;
}

/**
  * @brief  Jumping to memory address corresponding main program
  * @param  None
  * @retval None
  */
static void EnterProgram(void) 
{
    funct_ptr Program = (funct_ptr) *(volatile uint32_t *) (FIRMWARE_COPY_ADDR + 0x04);

    /* Setup the vector table to the final user-defined one in Flash
     * memory
     */
    WRITE_REG(SCB->VTOR, FIRMWARE_COPY_ADDR);
	
    /*
     * Setup the stack pointer to the user-defined one
     */
		//__set_MSP((*(volatile uint32_t *) FIRMWARE_COPY_ADDR));
		__ASM volatile ("MSR msp, %0" : : "r" ((*(volatile uint32_t *) FIRMWARE_COPY_ADDR)) : );
	
    // Use asm so stack is not used for branch
    __ASM volatile("bx %0\n\t"
    :
    : "r" (Program)
    :
    );
}
