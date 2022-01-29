/**
  ******************************************************************************
  * @file           : shift_registers.c
  * @brief          : Encoders driver implementation
		
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

#include "shift_registers.h"
#include "buttons.h"
#include "spi.h"


shift_reg_t shift_registers[4];

/**
  * @brief  Initializate shift registers states at startup
	* @param  p_dev_config: Pointer to device configuration
  * @retval None
  */
void ShiftRegistersInit(dev_config_t * p_dev_config)
{
	uint8_t pos = 0;
	int8_t prev_data = -1;
	int8_t prev_latch = -1;
	int8_t prev_clk = -1;

	for (int i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		shift_registers[i].pin_clk = -1;
		shift_registers[i].pin_data = -1;
		shift_registers[i].pin_latch = -1;
		shift_registers[i].button_cnt = p_dev_config->shift_registers[i].button_cnt;
		shift_registers[i].type = p_dev_config->shift_registers[i].type;
	}
	
	// set data pins
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == SHIFT_REG_DATA && i > prev_data)
		{
			shift_registers[pos].pin_data = i;				
			prev_data = i;
			pos++;			
		}
	}
	// set latch pins
	pos = 0;
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == SHIFT_REG_LATCH && i > prev_latch)
		{
			shift_registers[pos].pin_latch = i;					
			prev_latch = i;
			pos++;			
		}
	}
	// set clk pins
	pos = 0;
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		if (p_dev_config->pins[i] == SHIFT_REG_CLK && i > prev_clk)
		{
			shift_registers[pos].pin_clk = i;					
			prev_clk = i;
			pos++;			
		}
	}
	// if latch or clk pin not set and data pin is set than set last defined latch pin
	for (int i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		if (shift_registers[i].pin_data >= 0 && shift_registers[i].pin_latch == -1)
		{
			shift_registers[i].pin_latch = prev_latch;
		}
		if (shift_registers[i].pin_data >= 0 && shift_registers[i].pin_latch >= 0 &&  shift_registers[i].pin_clk == -1)
		{
			shift_registers[i].pin_clk = prev_clk;
		}
	}
}

/**
  * @brief  Read bytes from shift registers
	* @param  shift_register: Pointer to shift register configuration
	* @param  data: Pointer to data buffer
  * @retval None
  */
void ShiftRegisterRead(shift_reg_t * shift_register, uint8_t * data)
{
	uint8_t reg_cnt;
	
	if (shift_register->type == CD4021_PULL_DOWN || shift_register->type == CD4021_PULL_UP)		// positive polarity
	{
		// set SCK low
		pin_config[shift_register->pin_clk].port->ODR &= ~pin_config[shift_register->pin_clk].pin;
		// Latch impulse
		pin_config[shift_register->pin_latch].port->ODR |= pin_config[shift_register->pin_latch].pin;
		for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
		pin_config[shift_register->pin_latch].port->ODR &= ~pin_config[shift_register->pin_latch].pin;
			
	}
	else	// HC165 negative polarity
	{
		// set SCK high
		pin_config[shift_register->pin_clk].port->ODR |= pin_config[shift_register->pin_clk].pin;
		// Latch impulse
		pin_config[shift_register->pin_latch].port->ODR &= ~pin_config[shift_register->pin_latch].pin;
		for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
		pin_config[shift_register->pin_latch].port->ODR |= pin_config[shift_register->pin_latch].pin;			
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
				pin_config[shift_register->pin_clk].port->ODR &= ~pin_config[shift_register->pin_clk].pin;
				for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();				
				if(pin_config[shift_register->pin_data].port->IDR & pin_config[shift_register->pin_data].pin)
				{
					data[i] |= mask; 
				}
				pin_config[shift_register->pin_clk].port->ODR |= pin_config[shift_register->pin_clk].pin;		
				for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
				
				mask = mask >> 1;
			} while (mask);
		}
		else	// inverted connection
		{
			do
			{
				pin_config[shift_register->pin_clk].port->ODR &= ~pin_config[shift_register->pin_clk].pin;
				for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();
				if(!(pin_config[shift_register->pin_data].port->IDR & pin_config[shift_register->pin_data].pin))
				{
					data[i] |= mask; 
				}				
				pin_config[shift_register->pin_clk].port->ODR |= pin_config[shift_register->pin_clk].pin;
				for (int i=0; i<SHIFTREG_TICK_DELAY; i++) __NOP();

				mask = mask >> 1;
			} while (mask);
		}
	}
}

/**
  * @brief  Getting buttons states from shift registers
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void ShiftRegistersGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos)
{	
	uint8_t input_data[16];
	for (uint8_t i=0; i<MAX_SHIFT_REG_NUM; i++)
	{
		if (shift_registers[i].pin_latch >=0 && shift_registers[i].pin_data >=0 && shift_registers[i].pin_clk >= 0)
		{
			ShiftRegisterRead(&shift_registers[i], input_data);
			
			for (uint8_t j=0; j<shift_registers[i].button_cnt; j++)
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

