/**
  ******************************************************************************
  * @file           : crc16.c
  * @brief          : CRC-16 implementation
		
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

#include "crc16.h"

uint16_t crctable[256];

void MakeCrc16Table(void)
{
	uint16_t value;
	uint16_t temp;
	
  for(uint16_t i=0; i<256; ++i)
  {
    value = 0;
    temp = i;		
    for(uint8_t j=0; j<8; ++j)
    {
			if(((value ^ temp) & 0x0001) != 0)
			{				
				value=(value>>1)^POLINOM;
			}
			else	
			{
				value=value>>1;  	
			}
			temp >>= 1;
		}		
		crctable[i]=value;
   }
}

uint16_t Crc16(uint8_t* buf, uint16_t len)
{
	uint16_t crc = 0;
	
	MakeCrc16Table();
	
  for (uint16_t i=0; i<len; ++i)
	{
		uint8_t index = (crc ^ buf[i]);
		crc = (uint16_t) ((crc >> 8) ^ crctable[index]);
	}
	
  return crc;
}

