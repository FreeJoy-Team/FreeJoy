/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Modified January 2019
*	by Michel Stempin <michel.stempin@wanadoo.fr>
*	Cleanup and optimizations
*
*/

#include <stm32f10x.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "usb.h"
#include "hid.h"
#include "flash.h"
#include "crc16.h"

/* This should be <= MAX_EP_NUM defined in usb.h */
#define EP_NUM 			2

/* Maximum packet size */
#define MAX_PACKET_SIZE			64

#define FIRMWARE_DATA_SIZE	60

/* Buffer table offsset in PMA memory */
#define BTABLE_OFFSET		(0x00)

/* EP0  */
/* RX/TX buffer base address */
#define ENDP0_RXADDR		(0x18)
#define ENDP0_TXADDR		(0x58)

/* EP1  */
/* TX buffer base address */
#define ENDP1_TXADDR		(0x100)

/* Upload started flag */
volatile bool UploadStarted;

/* Upload finished flag */
volatile bool UploadFinished;

/* Hid buffer */

static uint8_t HidBuffer[MAX_PACKET_SIZE];
static volatile uint16_t FirmwareLength;
static volatile uint16_t FirmwareCrc;

/* USB Descriptors */
static const uint8_t USB_DeviceDescriptor[] = {
	0x12,			// bLength
	0x01,			// bDescriptorType (Device)
	0x10, 0x01,		// bcdUSB 1.10
	0x00,			// bDeviceClass (Use class information in the Interface Descriptors)
	0x00,			// bDeviceSubClass
	0x00,			// bDeviceProtocol
	MAX_PACKET_SIZE,	// bMaxPacketSize0 64
	0x83, 0x04,		// idVendor 0x0483 (0x1209)
	0xBA, 0xBE,		// idProduct 0xBEBA
	0x00, 0x03,		// bcdDevice 3.00
	0x01,			// iManufacturer (String Index)
	0x02,			// iProduct (String Index)
	0x00,			// iSerialNumber (String Index)
	0x01 			// bNumConfigurations 1
};

static const uint8_t USB_ConfigurationDescriptor[] = {
	0x09,			// bLength
	0x02,			// bDescriptorType (Configuration)
	0x22, 0x00,		// wTotalLength 34
	0x01,			// bNumInterfaces 1
	0x01,			// bConfigurationValue
	0x00,			// iConfiguration (String Index)
	0xC0,			// bmAttributes Self Powered
	0x32,			// bMaxPower 100mA

	0x09,			// bLength
	0x04,			// bDescriptorType (Interface)
	0x00,			// bInterfaceNumber 0
	0x00,			// bAlternateSetting
	0x01,			// bNumEndpoints 1
	0x03,			// bInterfaceClass
	0x00,			// bInterfaceSubClass
	0x00,			// bInterfaceProtocol
	0x00,			// iInterface (String Index)

	0x09,			// bLength
	0x21,			// bDescriptorType (HID)
	0x11, 0x01,		// bcdHID 1.11
	0x00,			// bCountryCode
	0x01,			// bNumDescriptors
	0x22,			// bDescriptorType[0] (HID)
	0x1A, 0x00,		// wDescriptorLength[0] 26

	0x07,			// bLength
	0x05,			// bDescriptorType (Endpoint)
	0x81,			// bEndpointAddress (IN/D2H)
	0x03,			// bmAttributes (Interrupt)
	MAX_PACKET_SIZE, 0x00,	// wMaxPacketSize 8
	0x05 			// bInterval 5 (2^(5-1)=16 micro-frames)
};

static const uint8_t USB_ReportDescriptor[26] = 
{
	0x06, 0x00, 0xFF,							 // Usage Page (Vendor Defined 0xFF00)
	0x09, 0x01,										 // Usage (0x01)
	0xA1, 0x01,										 // Collection (Application)
	
	0x85, 0x04,										 //   REPORT_ID (4)	
	0x09, 0x06,  									 //   USAGE (Vendor Usage 6)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
	
	0x09, 0x07,                    //   USAGE (Vendor Usage 7)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x95, 0x3f,                    //   REPORT_COUNT (63)
	0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
	0xc0,                          // END_COLLECTION
};

/* USB String Descriptors */
static const uint8_t USB_LangIDStringDescriptor[] = {
	0x04,			// bLength
	0x03,			// bDescriptorType (String)
	0x09, 0x04		// English (United States)
};

static const uint8_t USB_VendorStringDescriptor[] = {
	0x26,			// bLength
	0x03,			// bDescriptorType (String)
	/* Manufacturer: "STMicroelectronics" */
  'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
  'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
  'c', 0, 's', 0
};

static const uint8_t USB_ProductStringDescriptor[] = {
	0x26,			// bLength
	0x03,			// bDescriptorType (String)
	'F', 0, 'r', 0, 'e', 0, 'e', 0, 'J', 0, 'o', 0, 'y', 0, ' ', 0,
	'B', 0, 'o', 0, 'o', 0, 't', 0, 'l', 0, 'o', 0, 'a', 0, 'd', 0, 
	'e', 0, 'r', 0,
};

static void HIDUSB_GetDescriptor(USB_SetupPacket *setup_packet)
{
	uint16_t *descriptor = 0;
	uint16_t length = 0;

	switch (setup_packet->wValue.H) 
	{
		case USB_DEVICE_DESC_TYPE:
			descriptor =  (uint16_t *) USB_DeviceDescriptor;
			length = sizeof (USB_DeviceDescriptor);
			break;

		case USB_CFG_DESC_TYPE:
			descriptor = (uint16_t *) USB_ConfigurationDescriptor;
			length =  sizeof (USB_ConfigurationDescriptor);
			break;

		case USB_REPORT_DESC_TYPE:
			descriptor = (uint16_t *) USB_ReportDescriptor;
			length =  sizeof (USB_ReportDescriptor);
			break;

		case USB_STR_DESC_TYPE:
			switch (setup_packet->wValue.L) {
			case 0x00:
				descriptor = (uint16_t *) USB_LangIDStringDescriptor;
				length = sizeof (USB_LangIDStringDescriptor);
				break;

			case 0x01:
				descriptor = (uint16_t *) USB_VendorStringDescriptor;
				length = sizeof (USB_VendorStringDescriptor);
				break;

			case 0x02:
				descriptor = (uint16_t *) USB_ProductStringDescriptor;
				length = sizeof (USB_ProductStringDescriptor);
				break;

			default:
				break;
			}
			break;

		default:
			break;
	}
	if (length > setup_packet->wLength) 
	{
		length = setup_packet->wLength;
	}
	USB_SendData(0, descriptor, length);
}

static void HIDUSB_HandleData(uint8_t *data)
{

	uint16_t crc_comp = 0;
	uint16_t firmware_in_cnt = 0;
	uint16_t * write_address;
	
	memcpy(HidBuffer, data, MAX_PACKET_SIZE);
	
	if (HidBuffer[0] == 0x04)		// Repoer ID firmware
	{
		uint16_t cnt = HidBuffer[1]<<8 | HidBuffer[2];
		
		if (cnt == 0)			// first packet with info data
		{
			UploadStarted = true;
			FirmwareLength = HidBuffer[5]<<8 | HidBuffer[4];
			FirmwareCrc = HidBuffer[7]<<8 | HidBuffer[6];
			
			if (FirmwareLength <= 0xE000)	// check new firmware size
			{				
				LED1_ON;
				Flash_Unlock();
				for (uint8_t i=0; i<64 - MIN_PAGE; i++)
				{
					Flash_FormatPage((uint16_t * ) (USER_PROGRAM + i * PAGE_SIZE));					
				}
				
				Flash_Lock();
				LED1_OFF;
				
				firmware_in_cnt = cnt+1;
			}
			else // firmware size error
			{
				firmware_in_cnt = 0xF001;
			}
		}
		else if ( (FirmwareLength > 0) && (cnt*60 < FirmwareLength) )		// body of firmware data
		{
			LED1_ON;
			Flash_Unlock();
			
			write_address = (uint16_t * ) (USER_PROGRAM + (cnt)*60);
			FLASH_Write(write_address,(uint16_t *) &HidBuffer[4], MAX_PACKET_SIZE-4);
			
			Flash_Lock();
			LED1_OFF;
			firmware_in_cnt = cnt+1;
		}
		else if (FirmwareLength > 0)		// last packet
		{
			LED1_ON;
			Flash_Unlock();
			
			write_address = (uint16_t * ) (USER_PROGRAM + (cnt)*60);
			FLASH_Write(write_address,(uint16_t *) &HidBuffer[4], MAX_PACKET_SIZE-4);
			
			Flash_Lock();
			LED1_OFF;
			
			// check CRC16			
			crc_comp = Crc16((uint8_t*)USER_PROGRAM + 60, FirmwareLength);				
			if (FirmwareCrc == crc_comp && crc_comp != 0)
			{
				UploadFinished = true;
				firmware_in_cnt = 0xF000;	// OK
			}
			else	// CRC error
			{
				firmware_in_cnt = 0xF002;
			}
		}
		
		if (firmware_in_cnt > 0)
		{
			uint8_t tmp_buf[3];
			tmp_buf[0] = 4;		// REPORT_ID_FIRMWARE
			tmp_buf[1] = (firmware_in_cnt)>>8;
			tmp_buf[2] = (firmware_in_cnt)&0xFF;
			
			USB_SendData(ENDP0, (uint16_t *) tmp_buf, 3);
		}	
	}
}

void USB_Reset(void)
{
	/* Set buffer descriptor table offset in PMA memory */
	WRITE_REG(*BTABLE, BTABLE_OFFSET);

	/* Initialize Endpoint 0 */
	TOGGLE_REG(EP0REG[ENDP0],
		   EP_DTOG_RX | EP_T_FIELD | EP_KIND | EP_DTOG_TX | EPTX_STAT | EPADDR_FIELD,
		   0 | EP_CONTROL | 0,
		   EP_RX_VALID);

	/* Set transmission buffer address for endpoint 0 in buffer descriptor table */
	BTABLE_ADDR_FROM_OFFSET(ENDP0, BTABLE_OFFSET)[USB_ADDRn_TX] = ENDP0_TXADDR;

	/* Set reception buffer address for endpoint 0 in buffer descriptor table */
	BTABLE_ADDR_FROM_OFFSET(ENDP0, BTABLE_OFFSET)[USB_ADDRn_RX] = ENDP0_RXADDR;
	RxTxBuffer[0].MaxPacketSize = MAX_PACKET_SIZE;

	/* Initialize Endpoint 1 */
	TOGGLE_REG(EP0REG[ENDP1],
		   EP_DTOG_RX | EP_T_FIELD | EP_KIND | EP_DTOG_TX | EPADDR_FIELD,
		   1 | EP_INTERRUPT | 0,
		   EP_RX_DIS | EP_TX_NAK);

	/* Set transmission buffer address for endpoint 1 in buffer descriptor table */
	BTABLE_ADDR_FROM_OFFSET(ENDP1, BTABLE_OFFSET)[USB_ADDRn_TX] = ENDP1_TXADDR;

	/* Set transmission byte count for endpoint 1 in buffer descriptor table */
	BTABLE_ADDR_FROM_OFFSET(ENDP1, BTABLE_OFFSET)[USB_COUNTn_TX] = MAX_PACKET_SIZE;
	RxTxBuffer[1].MaxPacketSize = MAX_PACKET_SIZE;

	/* Clear device address and enable USB function */
	WRITE_REG(*DADDR, DADDR_EF | 0);
}

void USB_EPHandler(uint16_t status)
{
	uint8_t endpoint = READ_BIT(status, USB_ISTR_EP_ID);
	uint16_t endpoint_status = EP0REG[endpoint];
	USB_SetupPacket *setup_packet;

	/* OUT and SETUP packets (data reception) */
	if (READ_BIT(endpoint_status, EP_CTR_RX)) 
	{

		/* Copy from packet area to user buffer */
		USB_PMA2Buffer(endpoint);
		if (endpoint == 0)			
		{
			/* If control endpoint */
			if (READ_BIT(endpoint_status, USB_EP0R_SETUP)) 
			{
				setup_packet = (USB_SetupPacket *) RxTxBuffer[endpoint].RXB;
				switch (setup_packet->bRequest) 
				{
					case USB_REQUEST_SET_ADDRESS:
						DeviceAddress = setup_packet->wValue.L;
						USB_SendData(0, 0, 0);
						break;

					case USB_REQUEST_GET_DESCRIPTOR:
						HIDUSB_GetDescriptor(setup_packet);
						break;

					case USB_REQUEST_GET_STATUS:
						USB_SendData(0, (uint16_t *) &DeviceStatus, 2);
						break;

					case USB_REQUEST_GET_CONFIGURATION:
						USB_SendData(0, (uint16_t *) &DeviceConfigured, 1);
						break;

					case USB_REQUEST_SET_CONFIGURATION:
						DeviceConfigured = 1;
						USB_SendData(0, 0, 0);
						break;

					case USB_REQUEST_GET_INTERFACE:
						USB_SendData(0, 0, 0);
						break;

					default:
						USB_SendData(0, 0, 0);
						SET_TX_STATUS(ENDP0, EP_TX_STALL);
						break;
				}
			} 
			else if (RxTxBuffer[endpoint].RXL) 
			{
				/* OUT packet */
				HIDUSB_HandleData((uint8_t *) RxTxBuffer[endpoint].RXB);
			}

		}
		SET_RX_STATUS(endpoint, EP_RX_VALID);
	}
	if (READ_BIT(endpoint_status, EP_CTR_TX)) 
	{
		/* Something transmitted */
		if (DeviceAddress) 
		{
			/* Set device address and enable function */
			WRITE_REG(*DADDR, DADDR_EF | DeviceAddress);
			DeviceAddress = 0;
		}
		USB_Buffer2PMA(endpoint);
		SET_TX_STATUS(endpoint, (endpoint == ENDP1) ? EP_TX_NAK : EP_TX_VALID);
	}
}
