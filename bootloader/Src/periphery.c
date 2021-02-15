/**
  ******************************************************************************
  * @file           : periphery.c
  * @brief          : Periphery driver implementation
	
		
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

#include "periphery.h"

/* define compiler specific symbols */
#if defined ( __CC_ARM   )
#define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
#define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */

#elif defined ( __ICCARM__ )
#define __ASM           __asm                                       /*!< asm keyword for IAR Compiler          */
#define __INLINE        inline                                      /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

#elif defined   (  __GNUC__  )
#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */

#elif defined   (  __TASKING__  )
#define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
#define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */

#endif

void Delay(uint32_t timeout) {
    for (uint32_t i = 0; i < timeout; i++) {
        __NOP();
    }
}

/* IO init function */
void IO_Init(void) {
    /* GPIO Ports Clock Enable */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;

    // BOOT pin
    SET_BIT(GPIOB->CRL, GPIO_CRL_CNF2_0);
    CLEAR_BIT(GPIOB->CRL, GPIO_CRL_CNF2_1);
//    CLEAR_BIT(GPIOB->ODR, GPIO_ODR_ODR2);

    // LED pin
    SET_BIT(GPIOC->CRH, GPIO_CRH_MODE13);
		SET_BIT(GPIOB->CRH, GPIO_CRH_MODE12);
}

/**
  * @brief  Unlocks the FLASH Program Erase Controller.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function unlocks Bank1 and Bank2.
  *         - For all other devices it unlocks Bank1 and it is equivalent 
  *           to FLASH_UnlockBank1 function.. 
  * @param  None
  * @retval None
  */
void FLASH_Unlock(void) {
    /* Authorize the FPEC of Bank1 Access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while (FLASH->SR & FLASH_SR_BSY);
}

/**
  * @brief  Locks the FLASH Program Erase Controller.
  * @note   This function can be used for all STM32F10x devices.
  *         - For STM32F10X_XL devices this function Locks Bank1 and Bank2.
  *         - For all other devices it Locks Bank1 and it is equivalent 
  *           to FLASH_LockBank1 function.
  * @param  None
  * @retval None
  */
void FLASH_Lock(void) {
    /* Set the Lock Bit to lock the FPEC and the CR of  Bank1 */
    FLASH->CR |= CR_LOCK_Set;
}

/**
  * @brief  Erases a specified FLASH page.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Page_Address: The page address to be erased.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address) {
    /* Check the parameters */
    assert_param(IS_FLASH_ADDRESS(Page_Address));

    /* Wait for last operation to be completed */
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= CR_PG_Reset;
    /* if the previous operation is completed, proceed to erase the page */
    FLASH->CR |= CR_PER_Set;
    FLASH->AR = Page_Address;
    FLASH->CR |= CR_STRT_Set;

    /* Wait for last operation to be completed */
    while (FLASH->SR & FLASH_SR_BSY);

    /* Disable the PER Bit */
    FLASH->CR &= CR_PER_Reset;

    /* Return the Erase Status */
    return FLASH_COMPLETE;
}

/**
  * @brief  Programs a half word at a specified address.
  * @note   This function can be used for all STM32F10x devices.
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT. 
  */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data) {
    /* Check the parameters */
    assert_param(IS_FLASH_ADDRESS(Address));

    /* Wait for last operation to be completed */
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= CR_PER_Reset;
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR |= CR_PG_Set;

    *(__IO uint16_t *) Address = Data;
    /* Wait for last operation to be completed */
    while (FLASH->SR & FLASH_SR_BSY);

    /* Disable the PG Bit */
    FLASH->CR &= CR_PG_Reset;

    /* Return the Program Status */
    return FLASH_COMPLETE;
}
