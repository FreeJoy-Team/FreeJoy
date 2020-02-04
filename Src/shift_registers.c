/**
  ******************************************************************************
  * @file           : shift_registers.c
  * @brief          : Encoders driver implementation
  ******************************************************************************
  */

#include "shift_registers.h"
#include "buttons.h"

/**
  * @brief  Initializate shift registers states at startup
	* @param  p_config: Pointer to device configuration
  * @retval None
  */
void ShiftRegistersInit(app_config_t * p_config)
{
	uint8_t pos = 0;
	int8_t prev_cs = -1;
	int8_t prev_data = -1;
	
	for (int i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		p_config->shift_registers[i].pin_cs = -1;
		p_config->shift_registers[i].pin_data = -1;
	}
	
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_config->pins[i] == SHIFT_REG_CS && i > prev_cs)
		{
			for (int j=0; j<USED_PINS_NUM; j++)
			{
				if (p_config->pins[j] == SHIFT_REG_DATA && j > prev_data)
				{
					p_config->shift_registers[pos].pin_cs = i;
					p_config->shift_registers[pos].pin_data = j;
					
					prev_cs = i;
					prev_data = j;
					pos++;
					break;
				}
			}
		}
		
	}
}

/**
  * @brief  Read bytes from shift registers
	* @param  shift_register: Pointer to shift register configuration
	* @param  data: Pointer to data buffer
  * @retval None
  */
void ShiftRegisterRead(shift_reg_config_t * shift_register, uint8_t * data)
{
	uint8_t reg_cnt;
	
	GPIO_InitTypeDef 					GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_Init (GPIOB,&GPIO_InitStructure);
	// set SCK low
	GPIOB->ODR &= ~GPIO_Pin_3;
	
	if (shift_register->type == CD4021_PULL_DOWN || shift_register->type == CD4021_PULL_UP)		// positive polarity
	{
		// Latch impulse
		pin_config[shift_register->pin_cs].port->ODR |= pin_config[shift_register->pin_cs].pin;
		for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
		pin_config[shift_register->pin_cs].port->ODR &= ~pin_config[shift_register->pin_cs].pin;
			
	}
	else	// HC165 negative polarity
	{
		// Latch impulse
		pin_config[shift_register->pin_cs].port->ODR &= ~pin_config[shift_register->pin_cs].pin;
		for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
		pin_config[shift_register->pin_cs].port->ODR |= pin_config[shift_register->pin_cs].pin;			
	}
	
	
	reg_cnt = (uint8_t) ((float)shift_register->button_cnt/8.0);		// number of data bytes to read
	for (uint8_t i=0; i<reg_cnt; i++)
	{
		uint8_t mask = 0x80;
		
		data[i] = 0;
		
		if (shift_register->type == HC165_PULL_DOWN || shift_register->type == CD4021_PULL_DOWN)
		{
			do
			{
				GPIOB->ODR &= ~GPIO_Pin_3;			
				if(pin_config[shift_register->pin_data].port->IDR & pin_config[shift_register->pin_data].pin)
				{
					data[i] |= mask; 
				}
				GPIOB->ODR |= GPIO_Pin_3;
				
				mask = mask >> 1;
			} while (mask);
		}
		else	// inverted connection
		{
			do
			{
				GPIOB->ODR &= ~GPIO_Pin_3;			
				if(!(pin_config[shift_register->pin_data].port->IDR & pin_config[shift_register->pin_data].pin))
				{
					data[i] |= mask; 
				}
				GPIOB->ODR |= GPIO_Pin_3;
				
				mask = mask >> 1;
			} while (mask);
		}
	}
}

/**
  * @brief  Getting buttons states from shift registers
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void ShiftRegistersGet (uint8_t * raw_button_data_buf, app_config_t * p_config, uint8_t * pos)
{	
	uint8_t input_data[16];
	for (uint8_t i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		if (p_config->shift_registers[i].pin_cs >=0 && p_config->shift_registers[i].pin_data >=0)
		{
			ShiftRegisterRead(&p_config->shift_registers[i], input_data);
			for (uint8_t j=0; j<p_config->shift_registers[i].button_cnt; j++)
			{
				if ((*pos) <128)
				{
					raw_button_data_buf[(*pos)] = (input_data[(j & 0xF8)>>3] & (1<<(j & 0x07))) > 0 ? 1 : 0;
									
					(*pos)++;
				}
				else break;
			}

		}
	}
}

