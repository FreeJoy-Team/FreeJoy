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

// this code is taken from simhub arduino(SimHub\_Addons\Arduino\DisplayClientV2) and adapted for FJ and C language
// in SH_Read function, we are waiting for the next packet to be received
// this freezes all other calculations and needs to be improved!!
// but it is not an easy task


static const uint8_t crc_table_crc8[256] = { 0,213,127,170,254,43,129,84,41,252,86,131,215,2,168,125,82,135,45,248,172,121,211,6,123,174,4,209,133,80,250,47,164,113,219,14,90,143,37,240,141,88,242,39,115,166,12,217,246,35,137,92,8,221,119,162,223,10,160,117,33,244,94,139,157,72,226,55,99,182,28,201,180,97,203,30,74,159,53,224,207,26,176,101,49,228,78,155,230,51,153,76,24,205,103,178,57,236,70,147,199,18,184,109,16,197,111,186,238,59,145,68,107,190,20,193,149,64,234,63,66,151,61,232,188,105,195,22,239,58,144,69,17,196,110,187,198,19,185,108,56,237,71,146,189,104,194,23,67,150,60,233,148,65,235,62,106,191,21,192,75,158,52,225,181,96,202,31,98,183,29,200,156,73,227,54,25,204,102,179,231,50,152,77,48,229,79,154,206,27,177,100,114,167,13,216,140,89,243,38,91,142,36,241,165,112,218,15,32,245,95,138,222,11,161,116,9,220,118,163,247,34,136,93,214,3,169,124,40,253,87,130,255,42,128,85,1,212,126,171,132,81,251,46,122,175,5,208,173,120,210,7,83,134,44,249 };
#define updateCrc(currentCrc, value) crc_table_crc8[currentCrc ^ value]
	
static uint8_t partialdatabuffer[SH_PACKET_SIZE - 8];// in sh default = 24
static int16_t Arq_LastValidPacket = 255;
static int16_t packetID = -99;



//////////// Ring Buffer START
typedef struct
{
	uint8_t size;
	uint8_t read_index;
	uint8_t buffer[MAX_RING_BIF_SIZE];
} ring_buf_t;

// create Ring Buffer
ring_buf_t ring_buf;

/* DONT USE DIRECTLY */
uint8_t RB_WriteIndex(const ring_buf_t *b) 
{
	uint16_t wi = (uint16_t)b->read_index + (uint16_t)b->size;
	if (wi >= (uint16_t)MAX_RING_BIF_SIZE) wi -= (uint16_t)MAX_RING_BIF_SIZE;
	return (uint8_t)wi;
}

/* return the size of the buffer */
uint8_t RB_Size(ring_buf_t *b) { return b->size; }

/* access the buffer using array syntax, not interrupt safe */
uint8_t RB_MaxSize(ring_buf_t *b) { return MAX_RING_BIF_SIZE; }

/* Return true if the buffer is full */
uint8_t RB_IsFull(ring_buf_t *b) { return b->size == MAX_RING_BIF_SIZE; }

/* Return true if the buffer is empty */
uint8_t RB_IsEmpty(ring_buf_t *b) { return b->size == 0; }

/* Reset the buffer to an empty state */
void RB_Clear(ring_buf_t *b) { b->size = 0; }

/* Push a data at the end of the buffer. Return 1 if success, otherwise 0*/
uint8_t RB_Push(uint8_t value, ring_buf_t *b)
{
	if (RB_IsFull(b)) return 0;
	b->buffer[RB_WriteIndex(b)] = value;
	b->size++;
	return 1;
}

/* Push a array data at the end of the buffer. Return 1 if success, otherwise 0*/
uint8_t RB_PushArr(uint8_t *arr, uint8_t length, ring_buf_t *b)
{
	if (RB_IsFull(b) || b->size + length > MAX_RING_BIF_SIZE) return 0;
	memcpy(&b->buffer[RB_WriteIndex(b)], arr, length);
	b->size += length;
	return 1;
}

/* Pop the data at the beginning of the buffer and return it */
uint8_t RB_Pop(ring_buf_t *b)
{
	uint8_t value = b->buffer[b->read_index];
	b->read_index++;
	b->size--;
	if (b->read_index == MAX_RING_BIF_SIZE) b->read_index = 0;
	return value;
}

/* Pop the data at the beginning of the buffer. Return 1 if success, otherwise 0 */
uint8_t RB_PopErrCheck(uint8_t *value, ring_buf_t *b)
{
	if (RB_IsEmpty(b)) return 0;
	*value = b->buffer[b->read_index];
	b->read_index++;
	b->size--;
	if (b->read_index == MAX_RING_BIF_SIZE) b->read_index = 0;
	return 1;
}
/* Return pointer to Ring Buffer */
ring_buf_t * RB_GetPtr()
{
	return &ring_buf;
}
//////////// Ring Buffer STOP
	

	
void SH_SendAcq(uint8_t packetId)
{
	uint8_t data[2] = {0x03, packetId};
	CDC_Send_DATA ((uint8_t *)data, 2);
}

void SH_SendNAcq(uint8_t lastKnownValidPacket, uint8_t reason)
{
	uint8_t data[3] = {0x04, lastKnownValidPacket, reason};
	CDC_Send_DATA ((uint8_t *)data, 3);
}
	
	
void SH_ProcessIncomingData(uint8_t *data, uint8_t size) 
{
	int16_t length, header, res, i, crc, nextpacketid;
	uint8_t currentCrc;
	ring_buf_t *rb = RB_GetPtr();
	
	for (uint8_t pos = 0; pos < size; pos++)
	{
		header = data[pos++];
		currentCrc = 0;

		if (header == 0x01) {
			uint8_t reason = 0x00;

			header = data[pos++];
			if (header != 0x01) {
				return;
			}

			if (reason == 0) {
				packetID = data[pos++];
				if (packetID < 0) {
					reason = 0x01;
				}
			}

			if (reason == 0) {
				length = data[pos++];
				if (length <= 0 || length > SH_PACKET_SIZE) {
					reason = 0x02;
				}
			}

			if (reason == 0)
			{
				for (i = 0; i < length && !reason; i++) {
					res = data[pos++];
					partialdatabuffer[i] = res;
					if (res < 0) reason = 0x05;
				}
			}

			if (reason == 0) {
				crc = data[pos++];
				if (crc < 0) {
					reason = 0x03;
				}
			}

			if (reason == 0) {
				currentCrc = updateCrc(currentCrc, packetID);
				currentCrc = updateCrc(currentCrc, length);
				for (i = 0; i < length; i++) {
					currentCrc = updateCrc(currentCrc, partialdatabuffer[i]);
				}

				if (crc != currentCrc) {
					reason = 0x04;
				}
			}

			if (reason == 0) {
				nextpacketid = Arq_LastValidPacket > 127 ? 0 : Arq_LastValidPacket + 1;

				if (packetID == nextpacketid || packetID == 255) {
					for (i = 0; i < length; i++) {
						RB_Push(partialdatabuffer[i], rb);
					}
					Arq_LastValidPacket = packetID;
				}
				SH_SendAcq(packetID);
			}

			if (reason > 0) {
				SH_SendNAcq(Arq_LastValidPacket, reason);
			}
		}
	}
}


// TODO: in the current implementation, we are waiting for the next packet to be received
// this freezes all other calculations and needs to be fixed!!
// but it is not an easy task
int16_t SH_Read(void)
{
	int mil = GetMillis();
	ring_buf_t *rb = RB_GetPtr();
	do
	{
		if (RB_Size(rb) > 0)
		{
			return RB_Pop(rb);
		}
		
		if (CDC_IsReadeToSend()) SH_ProcessEndpData();
	}
	while (GetMillis() - mil < 400 || RB_Size(rb) > 0);
	
	return -1;
}

uint8_t SH_DataAvailable(void)
{
	ring_buf_t *rb = RB_GetPtr();
	if (RB_Size(rb) == 0 && CDC_IsReadeToSend())
	{
		SH_ProcessEndpData();
	}
	return RB_Size(rb);
}


// write one byte to simhub
void SH_writeByte(uint8_t value)
{
	uint8_t data[2] = {0x08, value};
	CDC_Send_DATA ((uint8_t *)data, sizeof(data));
}

// write char array to simhub
void SH_writeCharArr(const char *str, uint8_t length) // add length check
{
	uint8_t data[length + 7];
	data[0] = 0x06;
	data[1] = length;
	
	for(uint8_t i = 0; i < length; i++)
	{
		data[i + 2] = (uint8_t)str[i];
	}
	
	data[length + 2] = 0x20;
	data[length + 3] = 0x06;
	data[length + 4] = 1;
	data[length + 5] = '\n';
	data[length + 6] = 0x20;
	
	CDC_Send_DATA ((uint8_t *)data, sizeof(data));
}

// read simhub data up to terminator, fill char arr and return string length
uint8_t ReadStringUntil(char *str, char terminator1, char terminator2)
{
	int16_t c = SH_Read();
	uint8_t i = 0;
	
	while (c >= 0 && c != terminator1 && c != terminator2)
	{
		str[i++] = (char)c;
		c = SH_Read();
	}
	return i;
}


// RGB LEDs process
void SH_RGBDataProcess(RGB_t *rgb, uint8_t count) 
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t j;
	int mode = 1;
	mode = SH_Read();
	while (mode > 0)
	{
		// Read all
		if (mode == 1) {
			for (j = 0; j < count; j++) {
				r = SH_Read();
				g = SH_Read();
				b = SH_Read();

				rgb[j].r = r;
				rgb[j].g = g;
				rgb[j].b = b;
			}
		}

		// partial led data
		else if (mode == 2) {
			int startled = SH_Read();
			int numleds = SH_Read();

			for (j = startled; j < startled + numleds; j++) {
				r = SH_Read();
				g = SH_Read();
				b = SH_Read();

				rgb[j].r = r;
				rgb[j].g = g;
				rgb[j].b = b;
			}
		}

		// repeated led data
		else if (mode == 3) {
			int startled = SH_Read();
			int numleds = SH_Read();

			r = SH_Read();
			g = SH_Read();
			b = SH_Read();

			for (j = startled; j < startled + numleds; j++) {
				rgb[j].r = r;
				rgb[j].g = g;
				rgb[j].b = b;
			}
		}

		mode = SH_Read();
	}

	if (count > 0) {
		ws2812b_SendRGB(rgb, count);
	}
}


//////////// Commands START
void Command_Hello()
{
	SH_Read();
	SH_writeByte(SH_VERSION);
}
void Command_SetBaudrate()
{
	uint8_t br = SH_Read();
}
void Command_ButtonsCount()
{
	SH_writeByte(0);
}
void Command_TM1638Count()
{
	SH_writeByte(0);
}
void Command_SimpleModulesCount()
{
	SH_writeByte(0);
}
void Command_Acq()
{
	SH_writeByte(0x03);
}
void Command_DeviceName(char *name, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		if (name[i] == 0) {
			length = i+1;
		}
	}
	SH_writeCharArr(name, length);
}
void Command_UniqueId(char * id, uint8_t length)
{
	SH_writeCharArr(id, length);
}
void Command_Features()
{
	uint8_t data[16] = {0x06, 1, 'N', 0x20, 0x06, 1, 'I', 0x20, 0x06, 1, 'X', 0x20, 0x06, 1, '\n', 0x20};
	CDC_Send_DATA ((uint8_t *)data, 16);
}
void Command_TM1638Data(){}
void Command_Motors(){}
void Command_7SegmentsData(){}
	
void Command_RGBLEDSCount(uint8_t count)
{
	SH_writeByte(count);
}

void Command_RGBLEDSData(RGB_t *rgb, uint8_t count)
{
	SH_RGBDataProcess(rgb, count);
	// "while" - it seems necessary, but it works without
	int mil = GetMillis();	
	//do
	//{
		if (CDC_IsReadeToSend()) {
			SH_writeByte(0x15);
			//break;
		}
	//}
	//while (GetMillis() - mil < 20);
}

void Command_RGBMatrixData()
{
	SH_writeByte(0x15);
}
void Command_MatrixData(){}
void Command_GearData()
{
	uint8_t gear = SH_Read();
}
void Command_I2CLCDData(){}
void Command_GLCDData(){}
void Command_CustomProtocolData(){} // empty?
void Command_Shutdown(uint8_t leds_count)
{
	RGB_t rgb[leds_count];
	for (uint8_t i = 0; i < leds_count; i ++)
	{
		rgb[i].r = 0;
		rgb[i].g = 0;
		rgb[i].b = 0;
	}
	ws2812b_SendRGB(rgb, leds_count);
}
void Command_ExpandedCommandsList()
{
	uint8_t data[15] = {//0x06, 8, 'm', 'c', 'u', 't', 'y', 'p', 'e', '\n', 0x20,
											0x06, 10, 'k', 'e', 'e', 'p', 'a', 'l', 'i', 'v', 'e', '\n', 0x20,
											0x08, '\n'};
	CDC_Send_DATA ((uint8_t *)data, 15);
}
void Command_MCUType()
{
	char type[5] = "STM32";
	SH_writeCharArr(type, 5);
}
//////////// Commands STOP



void SH_Process(dev_config_t * p_dev_config, uint8_t * serial_num, uint8_t sn_length)
{
	static int64_t last_activity = 0;
	static uint8_t actived = 1;
	
	if (SH_DataAvailable() > 0 && CDC_IsReadeToSend()) {
		if (SH_Read() == SH_MESSAGE_HEADER)
		{
			last_activity = GetMillis();
			actived = 1;
			// Read command
			uint8_t loop_opt = SH_Read();

			if (loop_opt == '1') Command_Hello();
			else if (loop_opt == '8') Command_SetBaudrate();
			else if (loop_opt == 'J') Command_ButtonsCount();
			else if (loop_opt == '2') Command_TM1638Count();
			else if (loop_opt == 'B') Command_SimpleModulesCount();
			else if (loop_opt == 'A') Command_Acq();
			else if (loop_opt == 'N') Command_DeviceName(p_dev_config->device_name, sizeof(p_dev_config->device_name));
			else if (loop_opt == 'I') Command_UniqueId((char* )serial_num, sn_length);
			else if (loop_opt == '0') Command_Features();
			else if (loop_opt == '3') Command_TM1638Data();
			else if (loop_opt == 'V') Command_Motors();
			else if (loop_opt == 'S') Command_7SegmentsData();
			else if (loop_opt == '4') Command_RGBLEDSCount(p_dev_config->rgb_count);
			else if (loop_opt == '6') Command_RGBLEDSData(p_dev_config->rgb_leds, p_dev_config->rgb_count);
			else if (loop_opt == 'R') Command_RGBMatrixData();
			else if (loop_opt == 'M') Command_MatrixData();
			else if (loop_opt == 'G') Command_GearData();
			else if (loop_opt == 'L') Command_I2CLCDData();
			else if (loop_opt == 'K') Command_GLCDData(); // Nokia | OLEDS
			else if (loop_opt == 'P') Command_CustomProtocolData();
			else if (loop_opt == 'X')
			{
				char xaction[SH_PACKET_SIZE - 8];
				uint8_t length;
				length = ReadStringUntil(xaction, ' ', '\n');	// unsafe
				
				if (strncmp(xaction, "list", length) == 0) Command_ExpandedCommandsList();
				//else if (strncmp(xaction, "mcutype", length) == 0) Command_MCUType();
			}
		}
	}
	
	if (actived && (GetMillis() - last_activity > 5000)) {
		Command_Shutdown(p_dev_config->rgb_count);
		actived = 0;
	}
}
