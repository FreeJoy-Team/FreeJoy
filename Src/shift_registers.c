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
//	GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t reg_cnt;
	
//	// Configure SPI_SCK Pin as output PP
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// set SCK low
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
	
	if (shift_register->type == CD4021)		// positive polarity
	{
		// Latch impulse
		GPIO_WriteBit(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, Bit_SET);
		for (int i=0; i<5; i++) __NOP();
		GPIO_WriteBit(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, Bit_RESET);
			
	}
	else if (shift_register->type == HC165)		// negative polarity
	{
		// Latch impulse
		GPIO_WriteBit(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, Bit_RESET);
		for (int i=0; i<5; i++) __NOP();
		GPIO_WriteBit(pin_config[shift_register->pin_cs].port, pin_config[shift_register->pin_cs].pin, Bit_SET);			
	}
	
	
	reg_cnt = (uint8_t) ((float)shift_register->button_cnt/8.0);		// number of data bytes to read
	for (uint8_t i=0; i<reg_cnt; i++)
	{
		uint8_t mask = 0x80;
		
		data[i] = 0;
		do
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			for (int i=0; i<5; i++) __NOP();
			
			if(GPIO_ReadInputDataBit(pin_config[shift_register->pin_data].port, pin_config[shift_register->pin_data].pin) == Bit_RESET)
			{
				data[i] |= mask; 
			}
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			for (int i=0; i<5; i++) __NOP();
			
			mask = mask >> 1;
		} while (mask);
	}
	
	// Configure SPI_SCK Pin back to AF PP
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void ShiftRegistersProcess (buttons_state_t * button_state_buf, uint8_t * pov_buf, app_config_t * p_config, uint8_t * pos)
{	
	uint8_t input_data[16];
	for (uint8_t i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		if (p_config->shift_registers[i].pin_cs >=0 && p_config->shift_registers[i].pin_data >=0)
		{
			ShiftRegisterGet(&p_config->shift_registers[i], input_data);
			for (uint8_t j=0; j<p_config->shift_registers[i].button_cnt; j++)
			{
				if ((*pos) <128)
				{
					button_state_buf[(*pos)].pin_prev_state = button_state_buf[(*pos)].pin_state;
					button_state_buf[(*pos)].pin_state = (input_data[(j & 0xF8)>>3] & (1<<(j & 0x07))) > 0 ? 1 : 0;
					
					ButtonProcessState(&button_state_buf[(*pos)], pov_buf, p_config, pos);				
					(*pos)++;
				}
				else break;
			}

		}
	}
}

