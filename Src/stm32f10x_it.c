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
#include "tle5011.h"
#include "mcp320x.h"
#include "mlx90393.h"
#include "config.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define ADC_PERIOD_MS										2
#define SENSORS_PERIOD_MS								2
#define ENCODER_PERIOD_MS								1

/* Private variables ---------------------------------------------------------*/

volatile int32_t millis =0, joy_millis=0, encoder_millis = 0, adc_millis=100, sensors_millis=101;
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
		if (millis - joy_millis >= dev_config.exchange_period_ms )
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
		// Internal ADC conversion
		if (millis - adc_millis >= ADC_PERIOD_MS)
		{
			adc_millis = millis;
						
			AxesProcess(&dev_config);
			
			// Disable periphery before ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,DISABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, DISABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,DISABLE);			
				
			// ADC measurement
			for (uint8_t i=0; i<PREBUF_SIZE; i++)	ADC_Conversion();				// TODO: hide cycle in conversion function
			
			// Enable periphery after ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
			// Enable TLE clock after ADC conversion
			Generator_Start();
		}
		// External sensors data receiption
		if (millis - sensors_millis >= ADC_PERIOD_MS)
		{
			sensors_millis = millis;

			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
				if (sensors[i].cs_pin >= 0 && sensors[i].tx_complete && sensors[i].rx_complete)
				{
					if (sensors[i].type == TLE5011)
					{
						TLE501x_StartDMA(&sensors[i]);
						return;
					}
					else if (sensors[i].type == MCP3201 ||
									 sensors[i].type == MCP3202 ||
									 sensors[i].type == MCP3204 ||
									 sensors[i].type == MCP3208)
					{
						MCP320x_StartDMA(&sensors[i]);
						return;
					}
					else if (sensors[i].type == MLX90393)
					{
						MLX90393_StartDMA(&sensors[i++]);
						return;
					}
				}
			}
		}
		// encoders polling
		if (millis - encoder_millis >= ENCODER_PERIOD_MS)
		{
			encoder_millis = millis;
			EncoderProcess(buttons_state, &dev_config);
		}
		
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
			if (sensors[i].type == TLE5011)
			{
				TLE501x_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == MCP3201 ||
							 sensors[i].type == MCP3202 ||
							 sensors[i].type == MCP3204 ||
							 sensors[i].type == MCP3208)
			{
				MCP320x_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == MLX90393)
			{
				MLX90393_StopDMA(&sensors[i]);
				
				// search for last logical sensor for this physical sensor (in case of multiple channels)
				for (uint8_t k=0;k<MAX_AXIS_NUM;k++)
				{
					if (sensors[k].cs_pin == sensors[i].cs_pin)
					{
						memcpy(sensors[k].data, sensors[i].data, sizeof(sensors[k].data));
						i=k;
					}
				}
				i++;
			}
		}
		// Enable other peripery IRQs
		NVIC_EnableIRQ(TIM1_UP_IRQn);
		NVIC_EnableIRQ(TIM3_IRQn);		
		
		// Process next sensor
		for ( ;i<MAX_AXIS_NUM;i++)
		{
			if (sensors[i].cs_pin >= 0 && sensors[i].rx_complete && sensors[i].rx_complete)
			{
				if (sensors[i].type == TLE5011)
				{
					TLE501x_StartDMA(&sensors[i]);
					return;
				}
				else if (sensors[i].type == MCP3201 ||
								 sensors[i].type == MCP3202 ||
								 sensors[i].type == MCP3204 ||
								 sensors[i].type == MCP3208)
				{
					MCP320x_StartDMA(&sensors[i]);
					return;
				}
				else if (sensors[i].type == MLX90393)
				{
					// search for last logical sensor for this physical sensor (in case of multiple channels)
					for (uint8_t k=i;k<MAX_AXIS_NUM;k++)
					{
						if (sensors[k].cs_pin == sensors[i].cs_pin) i=k;
					}
					MLX90393_StartDMA(&sensors[i]);
					return;
				}
			}
		}
		// Disable TLE clock after communication frame
		Generator_Stop();
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
				if (sensors[i].type == TLE5011)
				{
					HardSPI_HalfDuplex_Receive(&sensors[i].data[1], 5);					
				}
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
