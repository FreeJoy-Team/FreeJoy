/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usb_istr.h"
#include "usb_lib.h"
#include "periphery.h"
#include "analog.h"
#include "encoders.h"
#include "sensors.h"
#include "config.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile int32_t millis =0, joy_millis=0;
extern dev_config_t dev_config;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	Ticks++;
		
	if (TimingDelay != 0x00)										
  {
    TimingDelay--;
  }
	
	
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

void TIM2_IRQHandler(void)
{	
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		TIM_Cmd(TIM2, DISABLE);
		NVIC_DisableIRQ(TIM2_IRQn);		
		
		NVIC_EnableIRQ(TIM3_IRQn);
		NVIC_EnableIRQ(TIM1_UP_IRQn);
	}
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		AxesProcess(&dev_config);

		
		for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
		{
			if (sensors[i].cs_pin >= 0 && sensors[i].rx_complete && sensors[i].rx_complete)
			{
				TLE501x_StartDMA(&sensors[i]);
				return;
			}
		}
		
	}	
}

void TIM1_UP_IRQHandler(void)
{
	uint8_t btn_num = 0;
	uint8_t	physical_buttons_data[MAX_BUTTONS_NUM];
	joy_report_t joy_report;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

		millis = GetTick();
		// check if it is time to send joystick data
		if (millis - joy_millis > dev_config.exchange_period_ms )
		{
			joy_millis = millis;
				
			// getting fresh data to joystick report buffer
			ButtonsGet(physical_buttons_data, joy_report.button_data, &joy_report.shift_button_data);
			AnalogGet(joy_report.axis_data, NULL, joy_report.raw_axis_data);	
			POVsGet(joy_report.pov_data);
			
			joy_report.raw_button_data[0] = btn_num;
			for (uint8_t i=0; i<64; i++)	
			{
				joy_report.raw_button_data[1 + ((i & 0xF8)>>3)] &= ~(1 << (i & 0x07));
				joy_report.raw_button_data[1 + ((i & 0xF8)>>3)] |= physical_buttons_data[btn_num+i] << (i & 0x07);
			}
			btn_num += 64;
			btn_num = btn_num & 0x7F;
			
			joy_report.id = REPORT_ID_JOY;	
							
			USB_CUSTOM_HID_SendReport((uint8_t *)&joy_report.id, sizeof(joy_report) - sizeof(joy_report.dummy));
		}
	
		EncoderProcess(buttons_state, &dev_config);
	}
	
}

// SPI Rx Complete
void DMA1_Channel2_IRQHandler(void)
{
	uint8_t i=0;
	
	if (DMA_GetITStatus(DMA1_IT_TC2))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC2);
		DMA_Cmd(DMA1_Channel2, DISABLE);
		
		// wait SPI transfer to end
		while(SPI1->SR & SPI_SR_BSY);
		
		// searching for active sensor
		for (i=0; i<MAX_AXIS_NUM; i++)
		{
			if (sensors[i].cs_pin >= 0 && !sensors[i].rx_complete) break;
		}
		// Close connection to the sensor
		if (i < MAX_AXIS_NUM)
		{
			TLE501x_StopDMA(&sensors[i++]);
		}
		// Enable other peripery IRQs
		NVIC_EnableIRQ(TIM1_UP_IRQn);
		NVIC_EnableIRQ(TIM3_IRQn);		
		
		// Process next sensor
		for ( ;i<MAX_AXIS_NUM;i++)
		{
			if (sensors[i].cs_pin >= 0 && sensors[i].rx_complete && sensors[i].rx_complete)
			{
				TLE501x_StartDMA(&sensors[i]);
				return;
			}
		}
	}
}

// SPI Tx Complete
void DMA1_Channel3_IRQHandler(void)
{
	uint8_t i=0;
	
	if (DMA_GetITStatus(DMA1_IT_TC3))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC3);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		
		// wait SPI transfer to end
		while(SPI1->SR & SPI_SR_BSY);
		
		// searching for active sensor
		for (i=0; i<MAX_AXIS_NUM; i++)
		{
			if (sensors[i].cs_pin >= 0 && !sensors[i].tx_complete)
			{
				sensors[i].tx_complete = 1;
				sensors[i].rx_complete = 0;
				UserSPI_HalfDuplex_Receive(&sensors[i].data[1], 5);
				return;
			}
		}
	}
}

/**
* @brief This function handles USB low priority or CAN RX0 interrupts.
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
