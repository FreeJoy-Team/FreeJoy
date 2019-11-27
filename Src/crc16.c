/**
  ******************************************************************************
  * @file           : crc16.c
  * @brief          : CRC-16 implementation
  ******************************************************************************
  */

#include "crc16.h"

uint16_t crctable[256];

//uint16_t crc16( uint8_t *buf, uint16_t num )
//{
//int i;
//uint16_t crc = 0xffff;

// while ( num-- )
// { 
//	 crc ^= *buf++;
//   i = 8;
//   do
//   { if ( crc & 1 )
//       crc = ( crc >> 1 ) ^ POLINOM;
//     else
//       crc >>= 1;
//   } while ( --i );
// }
// return( crc );

//}

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

