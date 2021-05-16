/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Descriptors for Custom HID Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

#include "common_defines.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t usb_vid = 0x0483;
uint16_t usb_pid = 0x5750;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
uint8_t Composite_DeviceDescriptor[Composite_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    0x83,            						/*idVendor = 0x0483 */
    0x04,
    0x50,						            /*idProduct = 0x5750*/
    0x57,            
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    1,                          /*Index of string descriptor describing
                                              manufacturer */
    2,                          /*Index of string descriptor describing
                                             product*/
    3,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* CustomHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
uint8_t Composite_ConfigDescriptor[Composite_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    Composite_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x02,         /* bNumInterfaces: 2 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0x80,         /* bmAttributes: Bus powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Joystick HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x01,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick HID HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    LOBYTE(0),		/* wItemLength: Total length of Report descriptor */
    HIBYTE(0),
    /******************** Descriptor of Joystick HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Joystick HID Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x40,          /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (1 ms) */
    /* 34 */
		
    /************** Descriptor of Custom HID interface ****************/
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x01,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
    /* 43 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    LOBYTE(CustomHID_SIZ_REPORT_DESC),/* wItemLength: Total length of Report descriptor */
    HIBYTE(CustomHID_SIZ_REPORT_DESC),
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 52 */
    0x07,          /* bLength: Custom HID Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x82,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x40,          /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x02,          /* bInterval: Polling Interval (2 ms)*/
    /* 59 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x02,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
		0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
		0x10,	/* bInterval: Polling Interval (16 ms) */
		/* 66 */
  }
  ; /* JoystickHID_ConfigDescriptor */
uint8_t JoystickHID_ReportDescriptor[JoystickHID_SIZ_REPORT_DESC] =
  {                    
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)

		0x85, REPORT_ID_JOY,				 	 //		REPORT_ID	(JOY_REPORT_ID)	

		// buttons data
		0x05, 0x09,                    //   USAGE_PAGE (Button)		
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, MAX_BUTTONS_NUM,         //   USAGE_MAXIMUM (Button MAX_BUTTONS_NUM)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, MAX_BUTTONS_NUM,         //   REPORT_COUNT (MAX_BUTTONS_NUM)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
		// axis data
		0x05, 0x01,                    // 	USAGE_PAGE (Generic Desktop)
		0x09, 0x30,                    //   USAGE (X)
    0x09, 0x31,                    //   USAGE (Y)
    0x09, 0x32,                    //   USAGE (Z)
    0x09, 0x33,                    //   USAGE (Rx)
    0x09, 0x34,                    //   USAGE (Ry)
    0x09, 0x35,                    //   USAGE (Rz)
		0x09, 0x36,                    //   USAGE (Slider)
		0x09, 0x36,                    //  	USAGE (Slider)
    0x16, 0x01, 0x80,              //  	LOGICAL_MINIMUM (-32767)
    0x26, 0xFF, 0x7F,						   //   LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, MAX_AXIS_NUM,       		 //   REPORT_COUNT (MAX_AXIS_NUM)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
		
		// POV data
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x15, 0x00, 									 //   LOGICAL_MINIMUM (0)
		0x25, 0x07, 									 //   LOGICAL_MAXIMUM (7)
		0x35, 0x00, 									 //   PHYSICAL_MINIMUM (0)
		0x46, 0x3B, 0x01,							 //   PHYSICAL_MAXIMUM (315)
		0x65, 0x12, 									 //   UNIT (SI Rot:Angular Pos) 
		0x75, 0x08, 									 //   REPORT_SIZE (8) 
		0x95, 0x01, 								   //   REPORT_COUNT (1)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)
		0x09, 0x39, 									 //   USAGE (Hat switch)
		0x81, 0x02, 									 //   INPUT (Data,Var,Abs)

		0xc0,                           // END_COLLECTION
  }; /* CustomHID_ReportDescriptor */
	
	  ; /* CustomHID_ConfigDescriptor */
uint8_t CustomHID_ReportDescriptor[CustomHID_SIZ_REPORT_DESC] =
  {                    
    0x06, 0x00, 0xFF,              // USAGE_PAGE (Vendor Defined 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)

		0x85, REPORT_ID_PARAM,				 	 //		REPORT_ID	(JOY_REPORT_ID)	  
		0x06, 0x00, 0xff,              // 	USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x02,                    //   USAGE (Vendor Usage 2)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
		0x09, 0x03,                    //   USAGE (Vendor Usage 3)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)

		// config data
		0x85, REPORT_ID_CONFIG_IN,     //   REPORT_ID (2)
		0x06, 0x00, 0xff,              // 	USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x04,                    //   USAGE (Vendor Usage 4)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x05,                    //   USAGE (Vendor Usage 5)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
    0x85, REPORT_ID_CONFIG_OUT,    //   REPORT_ID (3)	
    0x09, 0x06,                    //   USAGE (Vendor Usage 6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
		0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x07,                    //   USAGE (Vendor Usage 7)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x3f,                    //   REPORT_COUNT (63)
		0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
		0x85, REPORT_ID_FIRMWARE,    	 //   REPORT_ID (4)	
    0x09, 0x08,                    //   USAGE (Vendor Usage 8)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x02,                    //   REPORT_COUNT (2)
		0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
		
    0x09, 0x09,                    //   USAGE (Vendor Usage 9)
    0x75, 0x08,                    //   REPORT_SIZE (8)
		0x95, 0x3f,                    //   REPORT_COUNT (63)
		0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
		
		
		0xc0,                           // END_COLLECTION
  }; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
uint8_t Composite_StringLangID[Composite_SIZ_STRING_LANGID] =
  {
    Composite_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

uint8_t Composite_StringVendor[Composite_SIZ_STRING_VENDOR] =
  {
    Composite_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'F', 0, 'r', 0, 'e', 0, 'e', 0, 'J', 0, 'o', 0, 'y', 0
  };

uint8_t Composite_StringProduct[Composite_SIZ_STRING_PRODUCT] =
  {
    Composite_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
    'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
    'D', 0
  };
uint8_t Composite_StringSerial[Composite_SIZ_STRING_SERIAL] =
  {
    Composite_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

