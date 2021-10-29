/**
  ******************************************************************************
  * @file           : axes_to_buttons.c
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
uint8_t GetPressedFromAxis (uint8_t btn_num, axis_to_buttons_t * axis_to_btn, int32_t tmp)
{
	uint8_t ret = 0;
	if (btn_num == 0 && 
			tmp >= (uint32_t) (axis_to_btn->points[0] * AXIS_FULLSCALE) &&
			tmp <= (uint32_t) (axis_to_btn->points[1] * AXIS_FULLSCALE))
	{
		ret =  1;
	}	
	else if (btn_num == 1 && 
			tmp > (uint32_t) (axis_to_btn->points[1] * AXIS_FULLSCALE ) && 
			tmp <= (uint32_t) (axis_to_btn->points[2] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 2 && 
			tmp > (uint32_t) (axis_to_btn->points[2] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[3] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 3 && 
			tmp > (uint32_t) (axis_to_btn->points[3] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[4] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 4 && 
			tmp > (uint32_t) (axis_to_btn->points[4] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[5] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 5 && 
			tmp > (uint32_t) (axis_to_btn->points[5] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[6] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 6 && 
			tmp > (uint32_t) (axis_to_btn->points[6] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[7] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 7 && 
			tmp > (uint32_t) (axis_to_btn->points[7] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[8] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 8 && 
			tmp > (uint32_t) (axis_to_btn->points[8] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[9] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 9 && 
			tmp > (uint32_t) (axis_to_btn->points[9] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[10] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	else if (btn_num == 10 && 
			tmp > (uint32_t) (axis_to_btn->points[10] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[11] * AXIS_FULLSCALE ))
	{
		ret =  1;
	}
	else if (btn_num == 11 && 
			tmp > (uint32_t) (axis_to_btn->points[11] * AXIS_FULLSCALE) && 
			tmp <= (uint32_t) (axis_to_btn->points[12] * AXIS_FULLSCALE))
	{
		ret =  1;
	}
	
	return ret;
}

/**
  * @brief  Getting buttons states from axis to buttons
	* @param  raw_button_data_buf: Pointer to raw buttons data buffer
	* @param  p_dev_config: Pointer to device configuration
	* @param  pos: Pointer to button position counter
  * @retval None
  */
void AxisToButtonsGet (uint8_t * raw_button_data_buf, dev_config_t * p_dev_config, uint8_t * pos)
{
	analog_data_t 		scaled_axis_data[MAX_AXIS_NUM];
	
	// get axis data
	AnalogGet(NULL, scaled_axis_data, NULL);
	
	for (uint8_t i=0; i<MAX_AXIS_NUM; i++)
	{
		if (p_dev_config->axes_to_buttons[i].buttons_cnt > 0)
		{
			int32_t tmp = ((int32_t)scaled_axis_data[i] - AXIS_MIN_VALUE) * 255;
			for (uint8_t j=0; j<p_dev_config->axes_to_buttons[i].buttons_cnt; j++)
			{
				if ((*pos) < MAX_BUTTONS_NUM)
				{
					// get raw button state from axis
					raw_button_data_buf[*pos] = GetPressedFromAxis(j, &p_dev_config->axes_to_buttons[i], tmp);

					if (raw_button_data_buf[*pos] == 1)
					{
						uint8_t size = p_dev_config->axes_to_buttons[i].buttons_cnt - j;
						memset(&raw_button_data_buf[(*pos) +1], 0, size);
						*pos += size;
						break;
					}
					(*pos)++;
				}
				else break;
			}
		}
	}	
}
