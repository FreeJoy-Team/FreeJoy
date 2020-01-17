/**
  ******************************************************************************
  * @file           : periphery.c
  * @brief          : Periphery driver implementation
  ******************************************************************************
  */

#include "periphery.h"

SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim4;

pin_config_t pin_config[USED_PINS_NUM] =
{
	{GPIOA, GPIO_PIN_0, 0},					// 0
	{GPIOA, GPIO_PIN_1, 1},					// 1
	{GPIOA, GPIO_PIN_2, 2},					// 2
	{GPIOA, GPIO_PIN_3, 3},					// 3
	{GPIOA, GPIO_PIN_4, 4},					// 4
	{GPIOA, GPIO_PIN_5, 5},					// 5
	{GPIOA, GPIO_PIN_6, 6},					// 6
	{GPIOA, GPIO_PIN_7, 7},					// 7
	{GPIOA, GPIO_PIN_8, 8},					// 8
	{GPIOA, GPIO_PIN_9, 9},					// 9
	{GPIOA, GPIO_PIN_10, 10},				// 10	
	{GPIOA, GPIO_PIN_15, 15},				// 11
	{GPIOB, GPIO_PIN_0, 0},					// 12
	{GPIOB, GPIO_PIN_1, 1},					// 13
	{GPIOB, GPIO_PIN_3, 3},					// 14
	{GPIOB, GPIO_PIN_4, 4},					// 15
	{GPIOB, GPIO_PIN_5, 5},					// 16
	{GPIOB, GPIO_PIN_6, 6},					// 17
	{GPIOB, GPIO_PIN_7, 7},					// 18
	{GPIOB, GPIO_PIN_8, 8},					// 19
	{GPIOB, GPIO_PIN_9, 9},					// 20
	{GPIOB, GPIO_PIN_10, 10},				// 21
	{GPIOB, GPIO_PIN_11, 11},				// 22
	{GPIOB, GPIO_PIN_12, 12},				// 23
	{GPIOB, GPIO_PIN_13, 13},				// 24
	{GPIOB, GPIO_PIN_14, 14},				// 25
	{GPIOB, GPIO_PIN_15, 15},				// 26
	{GPIOC, GPIO_PIN_13, 13},				// 27
	{GPIOC, GPIO_PIN_14, 14},				// 28
	{GPIOC, GPIO_PIN_15, 15},				// 29
};

//void GetPinConfig (pin_config_t ** p_config)
//{
//	*p_config = pin_config;
//}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
void SPI1_Init(void)
{
	/* Peripheral clock enable */
  __HAL_RCC_SPI1_CLK_ENABLE(); 
	
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
	
  __HAL_AFIO_REMAP_SPI1_ENABLE();
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
void TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

	__HAL_RCC_TIM4_CLK_ENABLE();
	
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 18-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 9;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**TIM4 GPIO Configuration    
  PB6     ------> TIM4_CH1 
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

}

/* GPIO init function */
void GPIO_Init (app_config_t * p_config)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	while (p_config->firmware_version != FIRMWARE_VERSION)
	{
		// flash LED if firmware version doesnt match
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(300);
	}
	
	
	
	
	// setting up GPIO according confgiguration
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		// buttons
		if (p_config->pins[i] == BUTTON_GND)
		{
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == BUTTON_VCC)
		{
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == BUTTON_ROW)
		{
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == BUTTON_COLUMN)
		{
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}		
		else if (p_config->pins[i] == AXIS_ANALOG)
		{
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == SPI_SCK  && i == 14)
		{
			SPI1_Init();		// Half duplex SPI
			/**SPI1 GPIO Configuration    
			PB3     ------> SPI1_SCK
			*/
			GPIO_InitStruct.Pin = GPIO_PIN_3;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == TLE5011_DATA && i == 16)
		{
			//PB5     ------> SPI1_MOSI 
			GPIO_InitStruct.Pin = GPIO_PIN_5;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
		else if (p_config->pins[i] == TLE5011_CS || p_config->pins[i] == SHIFT_REG_CS)
		{
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
			HAL_GPIO_WritePin(pin_config[i].port, pin_config[i].pin, GPIO_PIN_SET);
		}
		else if (p_config->pins[i] == TLE5011_GEN  && i == 17)
		{
			TIM4_Init();	// 4MHz output at PB6 pin
		}
		else if (p_config->pins[i] == SHIFT_REG_DATA)
		{
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStruct.Pin = pin_config[i].pin;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(pin_config[i].port, &GPIO_InitStruct);
		}
		
	}

#ifdef DEBUG
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
}




