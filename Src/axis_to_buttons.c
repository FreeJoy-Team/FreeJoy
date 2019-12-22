/**
  ******************************************************************************
  * @file           : axis_to_buttons.c
  * @brief          : Axis to buttons driver implementation
  ******************************************************************************
  */

#include "axis_to_buttons.h"

analog_data_t scaled_axes_data[MAX_AXIS_NUM];
buttons_state_t buttons[MAX_AXIS_NUM * 11];

uint8_t GetPressedFromAxis (analog_data_t axis_data, uint8_t btn_num, axis_to_buttons_t axis_to_btn)
{
	if (axis_data >= (uint32_t) axis_to_btn.points[0] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[1] * AXIS_FULLSCALE / 100
			&& btn_num == 0)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[1] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[2] * AXIS_FULLSCALE / 100
			&& btn_num == 1)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[2] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[3] * AXIS_FULLSCALE / 100
			&& btn_num == 2)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[3] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[4] * AXIS_FULLSCALE / 100
			&& btn_num == 3)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[4] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[5] * AXIS_FULLSCALE / 100
			&& btn_num == 4)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[5] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[6] * AXIS_FULLSCALE / 100
			&& btn_num == 5)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[6] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[7] * AXIS_FULLSCALE / 100
			&& btn_num == 6)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[7] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[8] * AXIS_FULLSCALE / 100
			&& btn_num == 7)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[8] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[9] * AXIS_FULLSCALE / 100
			&& btn_num == 8)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[9] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[10] * AXIS_FULLSCALE / 100
			&& btn_num == 9)
	{
		return 1;
	}
	
	if (axis_data > (uint32_t) axis_to_btn.points[10] * AXIS_FULLSCALE / 100
			&& axis_data <= (uint32_t) axis_to_btn.points[11] * AXIS_FULLSCALE / 100
			&& btn_num == 10)
	{
		return 1;
	}
	
	return 0;
}

void AxesToButtonsProcess (buttons_state_t * button_state_buf, app_config_t * p_config, uint8_t start_index)
{
	uint8_t pos = start_index;
	
	AnalogGet(scaled_axes_data, NULL);
	
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		if (p_config->pins[i] == AXIS_TO_BUTTONS)
		{
			for (uint8_t j=0; j<p_config->axes_to_buttons[i].buttons_cnt; j++)
			{
				button_state_buf[pos++].current_state = GetPressedFromAxis(scaled_axes_data[i], j, p_config->axes_to_buttons[i]);
			}
		}
	}	
}


