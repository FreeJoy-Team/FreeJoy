/**
  ******************************************************************************
  * @file           : usb_endp.h
  * @brief          : Header for usb_endp.c file.
  ******************************************************************************
  */
	
	/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_ENDP_H__
#define __USB_ENDP_H__

#include "stdint.h"

int8_t USB_CUSTOM_HID_SendReport(uint8_t EP_num, uint8_t * data, uint8_t length);

uint8_t CDC_IsReadeToSend(void);
int8_t CDC_Send_DATA(uint8_t *ptrBuffer, uint8_t Send_length);

void SH_ProcessEndpData(void);

#endif 	/* __USB_ENDP_H__ */
