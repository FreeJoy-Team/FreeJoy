/**
  ******************************************************************************
  * @file           : shift_registers.c
  * @brief          : Encoders driver implementation
  ******************************************************************************
  */

#include "shift_registers.h"
#include "buttons.h"


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

// binbanging shift registers
void ShiftRegisterGet(shift_reg_config_t * shift_register, uint8_t * data)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t reg_cnt;
	uint8_t tmp8;
	
	// Configure SPI_SCK Pin as output PP
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// set SCK low
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	
	if (shift_register->type == CD4021)		// positive polarity
	{
		// Latch impulse
		HAL_GPIO_WritePin(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, GPIO_PIN_SET);
		for (int i=0; i<10; i++) __NOP();
		HAL_GPIO_WritePin(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, GPIO_PIN_RESET);
	}
	else if (shift_register->type == HC165)		// negative polarity
	{
		// Latch impulse
		HAL_GPIO_WritePin(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, GPIO_PIN_RESET);
		for (int i=0; i<10; i++) __NOP();
		HAL_GPIO_WritePin(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, GPIO_PIN_SET);
	}
	
	
	reg_cnt = (uint8_t) ((float)shift_register->button_cnt/8.0);		// number of data bytes to read
	for (uint8_t i=0; i<reg_cnt; i++)
	{
		data[i] = 0;
		for (uint8_t clk=0; clk<8; clk++)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			for (int i=0; i<10; i++) __NOP();
			
			tmp8 = HAL_GPIO_ReadPin(pin_config[shift_register->pin_data].port, pin_config[shift_register->pin_data].pin);
			data[i] |= (tmp8 << clk); 
			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			for (int i=0; i<10; i++) __NOP();
		}
	}
	
	// Configure SPI_SCK Pin back to AF PP
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void ShiftRegistersProcess (buttons_state_t * button_state_buf, uint8_t * pov_buf, app_config_t * p_config, uint8_t * pos)
{	
	uint32_t 					millis;
	uint8_t 					input_data[16];
	
	
	
	for (uint8_t i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		if (p_config->shift_registers[i].pin_cs >=0 && p_config->shift_registers[i].pin_data >=0)
		{
			ShiftRegisterGet(&p_config->shift_registers[i], input_data);
			
			for (uint8_t j=0; j<p_config->shift_registers[i].button_cnt; j++)
			{
				if (*pos <128)
				{
					button_state_buf[*pos].pin_prev_state = button_state_buf[*pos].pin_state;
					button_state_buf[*pos].pin_state = (input_data[(i & 0xF8)>>3] & (1<<(i & 0x07))) > 0 ? 1 : 0;
					
					ButtonProcessState(&button_state_buf[*pos], pov_buf, p_config, pos);				
					(*pos)++;
				}
				else break;
			}
		}
	}
}

