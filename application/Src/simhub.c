/**
  ******************************************************************************
  * @file           : simhub.c
  * @brief          : simhub data processing implementation
		
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
	
#include "simhub.h"
#include "usb_endp.h"
#include "periphery.h"
#include <stdio.h>//<string.h>

volatile uint8_t sh_start_led = 0;

//////////// Ring Buffer START
ring_buf_t ring_buf;

void RB_Push(uint8_t symbol, ring_buf_t* buf)
{
    buf->buffer[buf->idxIn++] = symbol;
    if (buf->idxIn >= buf->size) buf->idxIn = 0;
}

uint8_t RB_Pop(ring_buf_t *buf)
{
    uint8_t retval = buf->buffer[buf->idxOut++];
    if (buf->idxOut >= buf->size) buf->idxOut = 0;
    return retval;
}

uint16_t RB_Size(ring_buf_t *buf)
{
    uint16_t retval = 0;
    if (buf->idxIn < buf->idxOut) retval = buf->size + buf->idxIn - buf->idxOut;
    else retval = buf->idxIn - buf->idxOut;
    return retval;
}

void RB_Clear(ring_buf_t* buf)
{
    buf->idxIn = 0;
    buf->idxOut = 0;
}

uint16_t RB_MaxSize(ring_buf_t *b) { return b->size; }

RB_ErrorStatus_t RB_Init(ring_buf_t *ring, uint8_t *buf, uint16_t size)
{
    ring->size = size;
    ring->buffer = buf;
    RB_Clear( ring );

    return ( ring->buffer ? RING_SUCCESS : RING_ERROR ) ;
}

ring_buf_t * RB_GetPtr()
{
	return &ring_buf;
}
//////////// Ring Buffer END
	


// TODO: in the current implementation, we are waiting for the next packet to be received
// this freezes all other calculations and needs to be fixed!!
// but it is not an easy task
int16_t SH_Read(void)//////////////////////////////////////////
{
	
	int mil = GetMillis();
	ring_buf_t *rb = RB_GetPtr();
	do
	{
		if (RB_Size(rb) > 0)
		{
			return RB_Pop(rb);
		}
		
		SH_ProcessEndpData();
	}
	while (GetMillis() - mil < 400 || RB_Size(rb) > 0);
	
	return -1;
}


uint8_t SH_DataAvailable(void)
{
	ring_buf_t *rb = RB_GetPtr();
	SH_ProcessEndpData();
	return RB_Size(rb);
}


uint8_t SH_ProcessIncomingData(uint8_t *data, uint8_t size) 
{
	ring_buf_t *rb = RB_GetPtr();
	for (uint8_t i = 0; i < size; i++) {
		RB_Push(data[i], rb);
	}
	
	return RB_MaxSize(rb) - RB_Size(rb);
}

uint16_t SH_BufferFreeSize() 
{
	ring_buf_t *rb = RB_GetPtr();
	return RB_MaxSize(rb) - RB_Size(rb);
}




// write char array
void WriteLine(const char *str, uint8_t length) // add length check
{
	uint8_t data[length + 2];
	data[length] = '\r';
	data[length + 1] = '\n';
	
	for(uint8_t i = 0; i < length; i++)
	{
		data[i] = (uint8_t)str[i];
	}
	
	CDC_Send_DATA ((uint8_t *)data, sizeof(data));
}



void reverse(char s[])
{
	 int i, j;
	 char c;

	 for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
			 c = s[i];
			 s[i] = s[j];
			 s[j] = c;
	 }
}

void itoa(int n, char s[])
{
	 int i, sign;

	 if ((sign = n) < 0)
			 n = -n;
	 i = 0;
	 do {
			 s[i++] = n % 10 + '0';
	 } while ((n /= 10) > 0);
	 if (sign < 0)
			 s[i++] = '-';
	 s[i] = '\0';
	 reverse(s);
}
 


/// <summary>
/// Unlock upload protection.
/// </summary>
void unlockUpload() {
	WriteLine("Upload unlocked", 15);
}

/// <summary>
/// Sends leds count to the serial port.
/// </summary>
void getLedsCount(uint8_t ledCount) {
	char ch[3] = {0};
	itoa((int)ledCount, ch);/////////////////////////////////////////////// IntToUnicode?? usbhw.c
	//sprintf(ch,"%ld", (long)ledCount);
	WriteLine(ch, strlen(ch));
	
}

/// <summary>
/// Sends the protocol version.
/// </summary>
void getProtocolVersion() {
	char ch[10] = "SIMHUB_1.1";
	WriteLine(ch, sizeof(ch));
}
/// <summary>
/// Sends the protocol version.
/// </summary>
void getSerialNumber(char * id, uint8_t length) {
	WriteLine(id, length);
}

/// <summary>
/// Sends the protocol version.
/// </summary>
void resetSerialNumber(char * id, uint8_t length) {
	//ResetUniqueId();
	WriteLine(id, length);
}

/// <summary>
/// Read leds data from serial port.
/// </summary>
//volatile uint32_t nonv = 0; // debug
uint8_t readLeds(argb_led_t* rgb, uint8_t ledCount, uint8_t rightToLeft) {
	
	uint8_t r, g, b;
	for (uint8_t i = 0; i < ledCount; i++) {
		r = (char)SH_Read();
		g = (char)SH_Read();
		b = (char)SH_Read();
		if (rightToLeft) {
			rgb[ledCount - 1 - i].color.r = r;
			rgb[ledCount - 1 - i].color.g = g;
			rgb[ledCount - 1 - i].color.b = b;
		}
		else {
			rgb[i].color.r = r;
			rgb[i].color.g = g;
			rgb[i].color.b = b;
		}
	}

	// 3 bytes end of message, it must be (0xFF)(0xFE)(0xFD) to be taken into account, it's a super simple way to be sure we reached the correct end.
	uint8_t valid = 1;
	
	for (int i = 0; i < 3; i++) {
		char c = (char)SH_Read();
		valid = valid && (c == 0xFF - i);
	}

	if (valid) {
		return 1;
		//ws2812b_SendRGB(rgb, ledCount);
	}
//	} else { // debug
//		nonv++;
//	}
	return 0;
}




static uint8_t messageend = 0;
static char cmd[10]; // 5?
static uint8_t index_value = 0;

uint8_t SH_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length)
{
	ring_buf_t *rb = RB_GetPtr();
	uint8_t need_update = 0;
	
	if (SH_DataAvailable() > 0) {
		
		char c;
		do
		{
			c = (char)SH_Read();
			if (c == (char)0xFF) {
				messageend++;
			}
			else {
				if (messageend < 3 && c != (char)(0xff)) {
					messageend = 0;
				}
				break;
					//messageend = 0;
			}
		}
		while (messageend < 6 || SH_DataAvailable() > 0 || c != (char)(0xff));

		if (messageend >= 3 && c != (char)(0xff)) {
			cmd[index_value++] = c;
			//command += c;

			while (index_value < 5) {
				c = (char)SH_Read();
				cmd[index_value++] = c;
			}

			// Get protocol version
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)proto
			if (strncmp(cmd, "proto", 5) == 0) {
				getProtocolVersion();
			}

			// Get serial number
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)snumb
			if (strncmp(cmd, "snumb", 5) == 0) {
				getSerialNumber((char* )serial_num, sn_length);
			}

			// Reset serial number
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)snumb
			if (strncmp(cmd, "rnumb", 5) == 0) {
				resetSerialNumber((char* )serial_num, sn_length);
			}

			// Get leds count
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)ledsc
			if (strncmp(cmd, "ledsc", 5) == 0) {
				getLedsCount(p_dev_config->rgb_count);
			}

			// Send leds data (in binary) terminated by (0xFF)(0xFE)(0xFD)
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)sleds(RL1)(GL1)(BL1)(RL2)(GL2)(BL2) .... (0xFF)(0xFE)(0xFD)
			else if (strncmp(cmd, "sleds", 5) == 0) {
				need_update = readLeds(p_dev_config->rgb_leds, p_dev_config->rgb_count, 0);
				//sh_start_led = 1;
			}

			// Unlock upload
			// (0xFF)(0xFF)(0xFF)(0xFF)(0xFF)(0xFF)unloc
			else if (strncmp(cmd, "unloc", 5) == 0) {
				unlockUpload();
			}

			memset(cmd, 0, sizeof(cmd));
			index_value = 0;
			messageend = 0;
		}
	}
	return need_update;
}
