#include "usb_hw.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

ErrorStatus HSEStartUpStatus;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void AsciiToUnicode (uint8_t * pbuf_in , uint8_t *pbuf_out , uint8_t len);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	MODIFY_REG(GPIOA->CRH,
		GPIO_CRH_CNF12 | GPIO_CRH_MODE12,
		GPIO_CRH_CNF12_0);
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Enable the USB clock */
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  

  /* Enable the USB interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}


/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
  
  Device_Serial0 += Device_Serial2;
  
  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &CustomHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &CustomHID_StringSerial[18], 4);
  }
}


/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}


void USB_HW_Init(void)
{
	Set_System();

  USB_Interrupts_Config();

  Set_USBClock();

  USB_Init();
	
}

void USB_Shutdown(void)
{

	/* Disable USB IRQ */
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	WRITE_REG(*ISTR, 0);

	/* Turn USB Macrocell off */
	WRITE_REG(*CNTR, CNTR_FRES | CNTR_PDWN);

	/* PA12: General purpose output 50 MHz open drain */
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	MODIFY_REG(GPIOA->CRH,
		GPIO_CRH_CNF12 | GPIO_CRH_MODE12,
		GPIO_CRH_CNF12_0 | GPIO_CRH_MODE12);

	/* Sinks PA12 to GND */
	WRITE_REG(GPIOA->BRR, GPIO_BRR_BR12);

	/* Disable USB Clock on APB1 */
	CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_USBEN);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

