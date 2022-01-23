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
#include "tle5012.h"
#include "mcp320x.h"
#include "mlx90363.h"
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

#define ADC_PERIOD_TICKS										4					// 1 tick = 500us
#define SENSORS_PERIOD_TICKS								4
#define BUTTONS_PERIOD_TICKS								1
#define ENCODERS_PERIOD_TICKS								1

/* Private variables ---------------------------------------------------------*/

static joy_report_t 			joy_report;
static params_report_t 	params_report;

volatile int32_t millis = 0;
volatile int32_t joy_millis = 0; 
volatile int32_t encoder_ticks = 0;
volatile int32_t adc_ticks = 0;
volatile int32_t sensors_ticks = 1;
volatile int32_t buttons_ticks = 0;
volatile int32_t configurator_millis = 0;
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
	uint8_t						report_buf[64];
	uint8_t						pos = 0;
	app_config_t			tmp_app_config;
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		Ticks++;
		millis = GetMillis();
		
		
		
		// check if it is time to send joystick data
		if (millis - joy_millis >= dev_config.exchange_period_ms )
		{
			joy_millis = millis;

			AppConfigGet(&tmp_app_config);
				
			// getting fresh data to joystick report buffer
			ButtonsGet(joy_report.button_data, 
								 params_report.log_button_data, 
								 params_report.phy_button_data, 
								 &params_report.shift_button_data);
			AnalogGet(joy_report.axis_data, NULL, params_report.raw_axis_data);	
			POVsGet(joy_report.pov_data);
			
			// fill joystick report buffer
			report_buf[pos++] = REPORT_ID_JOY;			
			if (tmp_app_config.buttons_cnt > 0)
			{
				memcpy(&report_buf[pos], joy_report.button_data, MAX_BUTTONS_NUM/8);
				pos += (tmp_app_config.buttons_cnt - 1)/8 + 1;
			}
			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
					if (tmp_app_config.axis & (1<<i))
					{
						report_buf[pos++] = (uint8_t) (joy_report.axis_data[i] & 0xFF);
						report_buf[pos++] = (uint8_t) (joy_report.axis_data[i] >> 8);							
					}
			}
			for (uint8_t i=0; i<MAX_POVS_NUM; i++)
			{
					if (tmp_app_config.pov & (1<<i))
					{
						report_buf[pos++] = joy_report.pov_data[i];
					}
			}
			// send joystick report
			USB_CUSTOM_HID_SendReport(1, report_buf, pos);
		
			// fill params report buffer
			if (configurator_millis > millis)
			{
				static uint8_t report = 0;
				report_buf[0] = REPORT_ID_PARAM;
				params_report.firmware_version = FIRMWARE_VERSION;
				memcpy(params_report.axis_data, joy_report.axis_data, sizeof(params_report.axis_data));
				
				if (report == 0)
				{
					report_buf[1] = 0;
					memcpy(&report_buf[2], (uint8_t *)&(params_report), 62);
				}
				else
				{
					report_buf[1] = 1;
					memcpy(&report_buf[2], (uint8_t *)&(params_report) + 62, sizeof(params_report_t) - 62);
				}
				
				// send params report
				if (USB_CUSTOM_HID_SendReport(2, report_buf, 64) == 0)
				{
					report = !report;
				}
			}
		}

		// digital inputs polling
		if (Ticks - buttons_ticks >= BUTTONS_PERIOD_TICKS)
		{
			buttons_ticks = Ticks;
			ButtonsReadPhysical(&dev_config, raw_buttons_data);
			
			if (Ticks - encoder_ticks >= ENCODERS_PERIOD_TICKS)
			{
				encoder_ticks = Ticks;
				EncoderProcess(logical_buttons_state, &dev_config);
			}
		}
		
		// Internal ADC conversion
		if (Ticks - adc_ticks >= ADC_PERIOD_TICKS)
		{		
			adc_ticks = Ticks;	

			AxesProcess(&dev_config);					// process axis only once for one data reading
			
			// Disable periphery before ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,DISABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2|RCC_APB1Periph_TIM4, DISABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,DISABLE);
			
			if (tmp_app_config.fast_encoder_cnt == 0)
			{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,DISABLE);
			}
			if (tmp_app_config.pwm_cnt == 0)
			{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);
			}
				
			// ADC measurement
			ADC_Conversion();
			
			// Enable periphery after ADC conversion
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_TIM1,ENABLE);
		}
		// External sensors data receiption
		if (Ticks - sensors_ticks >= SENSORS_PERIOD_TICKS && Ticks > adc_ticks +1)		// prevent ADC and sensors reading during same ms
		{																																						
			sensors_ticks = Ticks;

			// start SPI sensors 
			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
				if (sensors[i].source >= 0 && sensors[i].tx_complete && sensors[i].rx_complete)
				{
					if (sensors[i].type == TLE5011)
					{
						TLE5011_StartDMA(&sensors[i]);
						break;
					}
					else if (sensors[i].type == TLE5012)
					{
						TLE5012_StartDMA(&sensors[i]);
						break;
					}
					else if (sensors[i].type == MCP3201 ||
									 sensors[i].type == MCP3202 ||
									 sensors[i].type == MCP3204 ||
									 sensors[i].type == MCP3208)
					{
						MCP320x_StartDMA(&sensors[i], 0);
						break;
					}
					else if (sensors[i].type == MLX90363)
					{
						MLX90363_StartDMA(&sensors[i]);
						break;
					}
					else if (sensors[i].type == MLX90393_SPI)
					{
						MLX90393_StartDMA(MLX_SPI, &sensors[i]);
						break;
					}
					else if (sensors[i].type == AS5048A_SPI)
					{
						AS5048A_StartDMA(&sensors[i]);
						break;
					}
				}
			}
			// start I2C sensors 
 			for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
			{
				if (sensors[i].source == (pin_t)SOURCE_I2C && sensors[i].rx_complete && sensors[i].tx_complete)
				{		
					if (sensors[i].type == AS5600)
					{
						status = AS5600_StartDMA(&sensors[i]);
						if (status != 0) continue;
						else break;
					}
					else if (sensors[i].type == ADS1115)
					{
						status = ADS1115_StartDMA(&sensors[i], sensors[i].curr_channel);	
						if (status != 0) continue;
						else break;
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
				TLE5011_StopDMA(&sensors[i++]);
			}
			else if (sensors[i].type == TLE5012)
			{
				TLE5012_StopDMA(&sensors[i++]);
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
			else if (sensors[i].type == MLX90363)
			{
				MLX90363_StopDMA(&sensors[i++]);
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
					TLE5011_StartDMA(&sensors[i]);
					return;
				}
				else if (sensors[i].type == TLE5012)
				{
					TLE5012_StartDMA(&sensors[i]);
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
				else if (sensors[i].type == MLX90363)
				{
					MLX90363_StartDMA(&sensors[i]);
					return;
				}
				else if (sensors[i].type == MLX90393_SPI)
				{
					MLX90393_StartDMA(MLX_SPI, &sensors[i]);
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
					SPI_HalfDuplex_Receive(&sensors[i].data[2], 6, TLE5011_SPI_MODE);					
				}
				if (sensors[i].type == TLE5012)
				{
					// switch MOSI back to open-drain
					GPIO_InitTypeDef GPIO_InitStructure;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;						
					GPIO_Init (GPIOB,&GPIO_InitStructure);
					
					SPI_HalfDuplex_Receive(&sensors[i].data[2], 4, TLE5012_SPI_MODE);					
				}
				break;
			}
		}
	}
}

// I2C Tx Complete (used for ADS1115 mux setting operation) 
void DMA1_Channel4_IRQHandler(void)
{
	uint8_t i=0;
	uint32_t ticks = I2C_TIMEOUT;
	
	if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
	{
		// Clear transmission complete flag 
		DMA_ClearFlag(DMA1_FLAG_TC4);
		
		I2C_DMACmd(I2C2,DISABLE);	
		DMA_Cmd(DMA1_Channel4,DISABLE);
		
		// EV8_2: Wait until BTF is set before programming the STOP
    while (((I2C2->SR1 & 0x00004) != 0x000004) && --ticks) {;}
		if(ticks == 0)	
		{
			sensors[i].tx_complete = 1;
			sensors[i].rx_complete = 1;
			return;
		}
		ticks = I2C_TIMEOUT;
		
    // Program the STOP
    I2C_GenerateSTOP(I2C2, ENABLE);
		
    /* Make sure that the STOP bit is cleared by Hardware */
		while ((I2C2->CR1&0x200) == 0x200 && --ticks);
		if (ticks == 0)	
		{
			sensors[i].tx_complete = 1;
			sensors[i].rx_complete = 1;
			return;
		}
		
		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			// searching for active sensor
			if (sensors[i].source == (pin_t)SOURCE_I2C && !sensors[i].tx_complete)
			{			
				sensors[i++].tx_complete = 1;			// TODO: check sensor disconnection				
				break;	
			}
		}
		
		// start processing for next I2C sensor 
		for (; i<MAX_AXIS_NUM; i++)
		{
				if (sensors[i].source == (pin_t)SOURCE_I2C && sensors[i].rx_complete && sensors[i].tx_complete)
				{		
					if (sensors[i].type == AS5600)
					{
						status = AS5600_StartDMA(&sensors[i]);
						if (status != 0) continue;
						else break;
					}
					else if (sensors[i].type == ADS1115)
					{
						status = ADS1115_StartDMA(&sensors[i], sensors[i].curr_channel);
						if (status != 0) continue;
						else break;
					}
				}
			}
	}
}

// I2C Rx Complete
void DMA1_Channel5_IRQHandler(void)
{
	uint8_t i=0;
	uint32_t ticks = I2C_TIMEOUT;
	
	if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
	{
		// Clear transmission complete flag 
		DMA_ClearFlag(DMA1_FLAG_TC5);
		
		I2C_DMACmd(I2C2,DISABLE);	
		DMA_Cmd(DMA1_Channel5,DISABLE);
		
		I2C_GenerateSTOP(I2C2, ENABLE);
		
		
		while ((I2C2->CR1&0x200) == 0x200 && --ticks);
		if (ticks == 0)	
		{
			sensors[i].tx_complete = 1;
			sensors[i].rx_complete = 1;
			return;
		}
		
		for (i = 0; i < MAX_AXIS_NUM; i++)
		{
			// searching for active sensor
			if (sensors[i].source == (pin_t)SOURCE_I2C && !sensors[i].rx_complete)
			{
				sensors[i].ok_cnt++;
				sensors[i].rx_complete = 1;		

				if (sensors[i].type == ADS1115)
				{
					// set mux to next channel
					uint8_t channel = (sensors[i].curr_channel < 3) ? (sensors[i].curr_channel + 1) : 0;
					status = ADS1115_SetMuxDMA(&sensors[i], channel);
				}
			}
		}
	}
}

// I2C error
void I2C2_ER_IRQHandler(void)
{
	__IO uint32_t SR1Register =0;

	/* Read the I2C2 status register */
	SR1Register = I2C2->SR1;
	/* If AF = 1 */
	if ((SR1Register & 0x0400) == 0x0400)
	{
		I2C2->SR1 &= 0xFBFF;
		SR1Register = 0;
	}
	/* If ARLO = 1 */
	if ((SR1Register & 0x0200) == 0x0200)
	{
		I2C2->SR1 &= 0xFBFF;
		SR1Register = 0;
	}
	/* If BERR = 1 */
	if ((SR1Register & 0x0100) == 0x0100)
	{
		I2C2->SR1 &= 0xFEFF;
		SR1Register = 0;
	}

	/* If OVR = 1 */
	if ((SR1Register & 0x0800) == 0x0800)
	{
		I2C2->SR1 &= 0xF7FF;
		SR1Register = 0;
	}
		
	// Reset I2C
	I2C2->CR1 |= I2C_CR1_SWRST;
	I2C2->CR1 &= ~I2C_CR1_SWRST;
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
