/**
  ******************************************************************************
  * @file           : axis_to_buttons.c
  * @brief          : Axis to buttons driver implementation
		
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

#include "axis_to_buttons.h"
#include "buttons.h"


/**
  * @brief  Getting buttons state from axis to buttons
	* @param  axis_data: Input axis data
	* @param  btn_num: Number of button in axis to buttons
	* @param  axis_to_btn: Pointer to axis to button configuration
  * @retval Button state
  */
uint8_t GetPressedFromAxis (analog_data_t axis_data, uint8_t btn_num, axis_to_buttons_t * axis_to_btn)
{
	uint8_t ret = 0;
	int32_t tmp = (int32_t)axis_data - AXIS_MIN_VALUE;
	
	if (tmp >= (uint32_t) (axis_to_btn->points[0] * AXIS_FULLSCALE) / 100 &&
			tmp <= (uint32_t) (axis_to_btn->points[1] * AXIS_FULLSCALE) / 100 &&
			btn_num == 0)
	{
		ret =  1;
	}	
	else if (tmp > (uint32_t) (axis_to_btn->points[1] * AXIS_FULLSCALE ) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[2] * AXIS_FULLSCALE) / 100 && 
			btn_num == 1)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[2] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[3] * AXIS_FULLSCALE) / 100 && 
			btn_num == 2)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[3] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[4] * AXIS_FULLSCALE) / 100 && 
			btn_num == 3)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[4] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[5] * AXIS_FULLSCALE) / 100 && 
			btn_num == 4)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[5] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[6] * AXIS_FULLSCALE) / 100 && 
			btn_num == 5)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[6] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[7] * AXIS_FULLSCALE) / 100 && 
			btn_num == 6)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[7] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[8] * AXIS_FULLSCALE) / 100 && 
			btn_num == 7)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[8] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[9] * AXIS_FULLSCALE) / 100 && 
			btn_num == 8)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[9] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[10] * AXIS_FULLSCALE) / 100 && 
			btn_num == 9)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[10] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[11] * AXIS_FULLSCALE ) / 100 && 
			btn_num == 10)
	{
		ret =  1;
	}
	else if (tmp > (uint32_t) (axis_to_btn->points[11] * AXIS_FULLSCALE) / 100 && 
			tmp <= (uint32_t) (axis_to_btn->points[12] * AXIS_FULLSCALE) / 100 && 
			btn_num == 11)
	{
		ret =  1;
	}
	
	return ret;
}

/**
  * @brief  Getting buttons states from axes to buttons
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void AxesToButtonsGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	analog_data_t 		scaled_axes_data[MAX_AXIS_NUM];
	
	// get axes data
	AnalogGet(NULL, scaled_axes_data, NULL);
	
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		if (p_dev_config->axes_to_buttons[i].is_enabled)
		{
			for (uint8_t j=0; j<p_dev_config->axes_to_buttons[i].buttons_cnt; j++)
			{
				if ((*pos) < MAX_BUTTONS_NUM)
				{
					// get raw button state from axis
					raw_button_data_buf[*pos] = GetPressedFromAxis(scaled_axes_data[i], j, &p_dev_config->axes_to_buttons[i]);

					(*pos)++;
				}
				else break;
			}
		}
	}	
}


