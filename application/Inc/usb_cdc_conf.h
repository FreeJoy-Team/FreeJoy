/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CDC_CONF_H
#define __USB_CDC_CONF_H

#include "usb_regs.h"
#include "usb_conf.h"

#define CDC_DATA_SIZE              							64		// when changing do not forget to recalculate TX/RXADDR
#define CDC_INT_SIZE               							8			// when changing do not forget to recalculate TX/RXADDR

// when changing endpoints do not forget to editing usb_conf.h and usb_endp.c
#define CDC_COMMAND_ENDP_ADR										EP3_IN
#define CDC_DATA_OUT_ENDP_ADR										EP4_OUT		// out = from host to device
#define CDC_DATA_IN_ENDP_ADR										EP5_IN		// in = from device to host

#define CDC_COMMAND_ENDP_NUM										ENDP3
#define CDC_DATA_OUT_ENDP_NUM										ENDP4
#define CDC_DATA_IN_ENDP_NUM										ENDP5

#define CDC_COMMAND_ENDP_BUF_ADR								ENDP3_TXADDR
#define CDC_DATA_OUT_ENDP_BUF_ADR								ENDP4_RXADDR
#define CDC_DATA_IN_ENDP_BUF_ADR								ENDP5_TXADDR


#endif /* __USB_CDC_CONF_H */
