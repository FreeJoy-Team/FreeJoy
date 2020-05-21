#ifndef __CONFIG_H
#define __CONFIG_H

#define MAX_FIRMWARE_SIZE		0xE000
/* Flash memory base address */
#define FLASH_BASE_ADDRESS	0x08000000

#define PAGE_SIZE		1024

#define MIN_PAGE		4

/* Bootloader size */
#define BOOTLOADER_SIZE			(MIN_PAGE * 1024)

/* SRAM size */
#define SRAM_SIZE						(20 * 1024)

/* SRAM end (bottom of stack) */
#define SRAM_END						(SRAM_BASE + SRAM_SIZE)

/* HID Bootloader takes 4 kb flash. */
#define USER_PROGRAM				(FLASH_BASE + BOOTLOADER_SIZE)

/* Initial stack pointer index in vector table*/
#define INITIAL_MSP					0

/* Reset handler index in vector table*/
#define RESET_HANDLER				1

/* USB Low-Priority and CAN1 RX0 IRQ handler idnex in vector table */
#define USB_LP_CAN1_RX0_IRQ_HANDLER	36

#define LED1_CLOCK		RCC_APB2ENR_IOPCEN
#define LED1_MODE		SET_BIT(GPIOC->CRH, GPIO_CRH_MODE13)
#define LED1_OFF			WRITE_REG(GPIOC->BSRR, GPIO_BSRR_BS13)
#define LED1_ON		WRITE_REG(GPIOC->BRR, GPIO_BRR_BR13)

// Enable the internal pull-down on PB2 pin. By default, PB2
// is in FLOATING input mode.
#define PB2_PULLDOWN


#ifndef LED1_CLOCK
#define LED1_CLOCK			0
#endif
#ifndef LED1_BIT_0
#define LED1_BIT_0
#endif
#ifndef LED1_BIT_1
#define LED1_BIT_1
#endif
#ifndef LED1_MODE
#define LED1_MODE
#endif
#ifndef LED1_ON
#define LED1_ON
#endif
#ifndef LED1_OFF
#define LED1_OFF
#endif

#endif
