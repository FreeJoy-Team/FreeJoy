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
#include "as5048a.h"
#include "ads1115.h"
#include "as5600.h"
#include "config.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define ADC_PERIOD_TICKS										2					// 1 tick = 1ms
#define SENSORS_PERIOD_TICKS								2
#define BUTTONS_PERIOD_TICKS								1

/* Private variables ---------------------------------------------------------*/

volatile int32_t ticks = 0;
volatile int32_t joy_ticks = 0; 
volatile int32_t encoder_ticks = 0;
volatile int32_t adc_ticks = 0;
volatile int32_t sensors_ticks = 1;
volatile int32_t buttons_ticks = 0;
volatile int status = 0;
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
	static uint8_t btn_num = 0;
	uint8_t	physical_buttons_data[MAX_BUTTONS_NUM];
	joy_report_t joy_report;
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		Ticks++;

		ticks = GetTick();
		// check if it is time to send joystick data
		if (ticks - joy_ticks >= dev_config.exchange_period_ms )
		{
			joy_ticks = ticks;
				
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

		// digital inputs polling
		if (ticks - encoder_ticks >= BUTTONS_PERIOD_TICKS)
		{
			ButtonsReadPhysical(&dev_config, raw_buttons_data);
			ButtonsDebouceProcess(&dev_config);
			
			encoder_ticks = ticks;
			EncoderProcess(logical_buttons_state, &dev_config);
		}
		
		// Internal ADC conversion
		if (ticks - adc_ticks >= ADC_PERIOD_TICKS)
		{		
			adc_ticks = ticks;	

			AxesProcess(&dev_config);					// process axes only once for one data reading
			
			// Disable periphery before ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,DISABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, DISABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,DISABLE);			
				
			// ADC measurement
			ADC_Conversion();
			
			// Enable periphery after ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
		}
		// External sensors data receiption
		if (ticks - sensors_ticks >= SENSORS_PERIOD_TICKS && ticks != adc_ticks)		// prevent ADC and sensors reading during same period
		{																																						
			sensors_ticks = ticks;

			// start I2C sensors 
 			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
				if (sensors[i].source == (pin_t)SOURCE_I2C && sensors[i].rx_complete && sensors[i].tx_complete)
				{		
					if (sensors[i].type == AS5600)
					{
						uint32_t tmp_pb = GPIOB->CRL;														// workaround of errata 2.9.8 issue
						GPIOB->CRL &= ~GPIO_CRL_MODE5;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
						status = AS5600_ReadBlocking(&sensors[i]);
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);		// workaround of errata 2.9.7 issue
						GPIOB->CRL = tmp_pb;
					}
					if (sensors[i].type == ADS1115)
					{
						uint32_t tmp_pb = GPIOB->CRL;														// workaround of errata 2.9.8 issue
						GPIOB->CRL &= ~GPIO_CRL_MODE5;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
						status = ADS1115_ReadBlocking(&sensors[i], sensors[i].curr_channel);					
						uint8_t channel = (sensors[i].curr_channel < 3) ? (sensors[i].curr_channel + 1) : 0;
						status = ADS1115_SetMuxBlocking(&sensors[i], channel);
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);		// workaround of errata 2.9.7 issue
						GPIOB->CRL = tmp_pb;
					}
				}
			}
			// start SPI sensors 
			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
				if (sensors[i].source >= 0 && sensors[i].tx_complete && sensors[i].rx_complete)
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
						MCP320x_StartDMA(&sensors[i], 0);
						return;
					}
					else if (sensors[i].type == MLX90393_SPI)
					{
						MLX90393_StartDMA(&sensors[i]);
						return;
					}
					else if (sensors[i].type == AS5048A_SPI)
					{
						AS5048A_StartDMA(&sensors[i]);
						return;
					}
				}
			}
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
			if (sensors[i].source >= 0 && !sensors[i].rx_complete) break;
		}
		// Close connection to the sensor
		if (i < MAX_AXIS_NUM)
		{
			if (sensors[i].type == TLE5011)
			{
				TLE501x_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == MCP3201)
			{
				MCP320x_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == MCP3202)
			{
				MCP320x_StopDMA(&sensors[i]);
				// get data from next channel
				if (sensors[i].curr_channel < 1)	
				{
					MCP320x_StartDMA(&sensors[i], sensors[i].curr_channel + 1);
					return;
				}
				i++;
			}	
			else if (sensors[i].type == MCP3204)
			{
				MCP320x_StopDMA(&sensors[i]);
				// get data from next channel
				if (sensors[i].curr_channel < 3)	
				{
					MCP320x_StartDMA(&sensors[i], sensors[i].curr_channel + 1);
					return;
				}
				i++;
			}	
			else if (sensors[i].type == MCP3208)
			{
				MCP320x_StopDMA(&sensors[i]);
				// get data from next channel
				if (sensors[i].curr_channel < 7)	
				{
					MCP320x_StartDMA(&sensors[i], sensors[i].curr_channel + 1);
					return;
				}
				i++;
			}
			else if (sensors[i].type == MLX90393_SPI)
			{
				MLX90393_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == AS5048A_SPI)
			{
				AS5048A_StopDMA(&sensors[i++]);
			}
		}
		
		
		// Process next sensor
		for ( ;i<MAX_AXIS_NUM;i++)
		{
			if (sensors[i].source >= 0 && sensors[i].rx_complete && sensors[i].tx_complete)
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
					MCP320x_StartDMA(&sensors[i], 0);
					return;
				}
				else if (sensors[i].type == MLX90393_SPI)
				{
					MLX90393_StartDMA(&sensors[i]);
					return;
				}
				else if (sensors[i].type == AS5048A_SPI)
				{
					AS5048A_StartDMA(&sensors[i]);
					return;
				}
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
		while(!SPI1->SR & SPI_SR_TXE);
		while(SPI1->SR & SPI_SR_BSY);
		
		// searching for active sensor
		for (i=0; i<MAX_AXIS_NUM; i++)
		{
			if (sensors[i].source >= 0 && !sensors[i].tx_complete)
			{
				sensors[i].tx_complete = 1;
				sensors[i].rx_complete = 0;
				if (sensors[i].type == TLE5011)
				{
					SPI_HalfDuplex_Receive(&sensors[i].data[1], 5, TLE5011_SPI_MODE);					
				}
				return;
			}
		}
	}
}

// I2C error
void I2C1_ER_IRQHandler(void)
{
	__IO uint32_t SR1Register =0;

	/* Read the I2C1 status register */
	SR1Register = I2C1->SR1;
	/* If AF = 1 */
	if ((SR1Register & 0x0400) == 0x0400)
	{
		I2C1->SR1 &= 0xFBFF;
		SR1Register = 0;
	}
	/* If ARLO = 1 */
	if ((SR1Register & 0x0200) == 0x0200)
	{
		I2C1->SR1 &= 0xFBFF;
		SR1Register = 0;
	}
	/* If BERR = 1 */
	if ((SR1Register & 0x0100) == 0x0100)
	{
		I2C1->SR1 &= 0xFEFF;
		SR1Register = 0;
	}

	/* If OVR = 1 */
	if ((SR1Register & 0x0800) == 0x0800)
	{
		I2C1->SR1 &= 0xF7FF;
		SR1Register = 0;
	}
		
	// Reset I2C
	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~I2C_CR1_SWRST;
	I2C_Start();
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
