/**
  ******************************************************************************
  * @file    usb_regs.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Interface functions to USB cell registers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * Function Name  : SetCNTR.
  * Description    : Set the CNTR register value.
  * Input          : wRegValue: new register value.
  * Output         : None.
  * Return         : None.
  **/
void SetCNTR(uint16_t wRegValue)
{
  _SetCNTR(wRegValue);
}

/**
  * Function Name  : GetCNTR.
  * Description    : returns the CNTR register value.
  * Input          : None.
  * Output         : None.
  * Return         : CNTR register Value.
  **/
uint16_t GetCNTR(void)
{
  return(_GetCNTR());
}

/**
  * Function Name  : SetISTR.
  * Description    : Set the ISTR register value.
  * Input          : wRegValue: new register value.
  * Output         : None.
  * Return         : None.
  **/
void SetISTR(uint16_t wRegValue)
{
  _SetISTR(wRegValue);
}

/**
  * Function Name  : GetISTR
  * Description    : Returns the ISTR register value.
  * Input          : None.
  * Output         : None.
  * Return         : ISTR register Value
  **/
uint16_t GetISTR(void)
{
  return(_GetISTR());
}

/**
  * Function Name  : GetFNR
  * Description    : Returns the FNR register value.
  * Input          : None.
  * Output         : None.
  * Return         : FNR register Value
  **/
uint16_t GetFNR(void)
{
  return(_GetFNR());
}

/**
  * Function Name  : SetDADDR
  * Description    : Set the DADDR register value.
  * Input          : wRegValue: new register value.
  * Output         : None.
  * Return         : None.
  **/
void SetDADDR(uint16_t wRegValue)
{
  _SetDADDR(wRegValue);
}

#ifdef LPM_ENABLED
/**
  * @brief Set the LPMCSR register value
  * @param   wRegValue: new register value
  * @retval None
  */
void SetLPMCSR(uint16_t wRegValue)
{
  _SetLPMCSR(wRegValue);
}


/**
  * @brief Returns the LPMCSR register value
  * @param   None
  * @retval LPMCSR register Value
  */
uint16_t GetLPMCSR(void)
{
  return(_GetLPMCSR());
}

#endif /* LPM_ENABLED */
/**
  * Function Name  : GetDADDR
  * Description    : Returns the DADDR register value.
  * Input          : None.
  * Output         : None.
  * Return         : DADDR register Value
  **/
uint16_t GetDADDR(void)
{
  return(_GetDADDR());
}

/**
  * Function Name  : SetBTABLE
  * Description    : Set the BTABLE.
  * Input          : wRegValue: New register value.
  * Output         : None.
  * Return         : None.
  **/
void SetBTABLE(uint16_t wRegValue)
{
  _SetBTABLE(wRegValue);
}

/**
  * Function Name  : GetBTABLE.
  * Description    : Returns the BTABLE register value.
  * Input          : None. 
  * Output         : None.
  * Return         : BTABLE address.
  **/
uint16_t GetBTABLE(void)
{
  return(_GetBTABLE());
}

/**
  * Function Name  : SetENDPOINT
  * Description    : Set the Endpoint register value.
  * Input          : bEpNum: Endpoint Number. 
  *                  wRegValue.
  * Output         : None.
  * Return         : None.
  **/
void SetENDPOINT(uint8_t bEpNum, uint16_t wRegValue)
{
  _SetENDPOINT(bEpNum, wRegValue);
}

/**
  * Function Name  : GetENDPOINT
  * Description    : Return the Endpoint register value.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Endpoint register value.
  **/
uint16_t GetENDPOINT(uint8_t bEpNum)
{
  return(_GetENDPOINT(bEpNum));
}

/**
  * Function Name  : SetEPType
  * Description    : sets the type in the endpoint register.
  * Input          : bEpNum: Endpoint Number. 
  *                  wType: type definition.
  * Output         : None.
  * Return         : None.
  **/
void SetEPType(uint8_t bEpNum, uint16_t wType)
{
  _SetEPType(bEpNum, wType);
}

/**
  * Function Name  : GetEPType
  * Description    : Returns the endpoint type.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Endpoint Type
  **/
uint16_t GetEPType(uint8_t bEpNum)
{
  return(_GetEPType(bEpNum));
}

/**
  * Function Name  : SetEPTxStatus
  * Description    : Set the status of Tx endpoint.
  * Input          : bEpNum: Endpoint Number. 
  *                  wState: new state.
  * Output         : None.
  * Return         : None.
  **/
void SetEPTxStatus(uint8_t bEpNum, uint16_t wState)
{
  _SetEPTxStatus(bEpNum, wState);
}

/**
  * Function Name  : SetEPRxStatus
  * Description    : Set the status of Rx endpoint.
  * Input          : bEpNum: Endpoint Number. 
  *                  wState: new state.
  * Output         : None.
  * Return         : None.
  **/
void SetEPRxStatus(uint8_t bEpNum, uint16_t wState)
{
  _SetEPRxStatus(bEpNum, wState);
}

/**
  * Function Name  : SetDouBleBuffEPStall
  * Description    : sets the status for Double Buffer Endpoint to STALL
  * Input          : bEpNum: Endpoint Number. 
  *                  bDir: Endpoint direction.
  * Output         : None.
  * Return         : None.
  **/
void SetDouBleBuffEPStall(uint8_t bEpNum, uint8_t bDir)
{
  uint16_t Endpoint_DTOG_Status;
  Endpoint_DTOG_Status = GetENDPOINT(bEpNum);
  if (bDir == EP_DBUF_OUT)
  { /* OUT double buffered endpoint */
    _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPRX_DTOG1);
  }
  else if (bDir == EP_DBUF_IN)
  { /* IN double buffered endpoint */
    _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPTX_DTOG1);
  }
}

/**
  * Function Name  : GetEPTxStatus
  * Description    : Returns the endpoint Tx status.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Endpoint TX Status
  **/
uint16_t GetEPTxStatus(uint8_t bEpNum)
{
  return(_GetEPTxStatus(bEpNum));
}

/**
  * Function Name  : GetEPRxStatus
  * Description    : Returns the endpoint Rx status.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Endpoint RX Status
  **/
uint16_t GetEPRxStatus(uint8_t bEpNum)
{
  return(_GetEPRxStatus(bEpNum));
}

/**
  * Function Name  : SetEPTxValid
  * Description    : Valid the endpoint Tx Status.
  * Input          : bEpNum: Endpoint Number.  
  * Output         : None.
  * Return         : None.
  **/
void SetEPTxValid(uint8_t bEpNum)
{
  _SetEPTxStatus(bEpNum, EP_TX_VALID);
}

/**
  * Function Name  : SetEPRxValid
  * Description    : Valid the endpoint Rx Status.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void SetEPRxValid(uint8_t bEpNum)
{
  _SetEPRxStatus(bEpNum, EP_RX_VALID);
}

/**
  * Function Name  : SetEP_KIND
  * Description    : Clear the EP_KIND bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void SetEP_KIND(uint8_t bEpNum)
{
  _SetEP_KIND(bEpNum);
}

/**
  * Function Name  : ClearEP_KIND
  * Description    : set the  EP_KIND bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearEP_KIND(uint8_t bEpNum)
{
  _ClearEP_KIND(bEpNum);
}

/**
  * Function Name  : Clear_Status_Out
  * Description    : Clear the Status Out of the related Endpoint
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void Clear_Status_Out(uint8_t bEpNum)
{
  _ClearEP_KIND(bEpNum);
}

/**
  * Function Name  : Set_Status_Out
  * Description    : Set the Status Out of the related Endpoint
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void Set_Status_Out(uint8_t bEpNum)
{
  _SetEP_KIND(bEpNum);
}

/**
  * Function Name  : SetEPDoubleBuff
  * Description    : Enable the double buffer feature for the endpoint. 
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void SetEPDoubleBuff(uint8_t bEpNum)
{
  _SetEP_KIND(bEpNum);
}

/**
  * Function Name  : ClearEPDoubleBuff
  * Description    : Disable the double buffer feature for the endpoint. 
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearEPDoubleBuff(uint8_t bEpNum)
{
  _ClearEP_KIND(bEpNum);
}

/**
  * Function Name  : GetTxStallStatus
  * Description    : Returns the Stall status of the Tx endpoint.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Tx Stall status.
  **/
uint16_t GetTxStallStatus(uint8_t bEpNum)
{
  return(_GetTxStallStatus(bEpNum));
}

/**
  * Function Name  : GetRxStallStatus
  * Description    : Returns the Stall status of the Rx endpoint. 
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Rx Stall status.
  **/
uint16_t GetRxStallStatus(uint8_t bEpNum)
{
  return(_GetRxStallStatus(bEpNum));
}

/**
  * Function Name  : ClearEP_CTR_RX
  * Description    : Clear the CTR_RX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearEP_CTR_RX(uint8_t bEpNum)
{
  _ClearEP_CTR_RX(bEpNum);
}

/**
  * Function Name  : ClearEP_CTR_TX
  * Description    : Clear the CTR_TX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearEP_CTR_TX(uint8_t bEpNum)
{
  _ClearEP_CTR_TX(bEpNum);
}

/**
  * Function Name  : ToggleDTOG_RX
  * Description    : Toggle the DTOG_RX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ToggleDTOG_RX(uint8_t bEpNum)
{
  _ToggleDTOG_RX(bEpNum);
}

/**
  * Function Name  : ToggleDTOG_TX
  * Description    : Toggle the DTOG_TX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ToggleDTOG_TX(uint8_t bEpNum)
{
  _ToggleDTOG_TX(bEpNum);
}

/**
  * Function Name  : ClearDTOG_RX.
  * Description    : Clear the DTOG_RX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearDTOG_RX(uint8_t bEpNum)
{
  _ClearDTOG_RX(bEpNum);
}

/**
  * Function Name  : ClearDTOG_TX.
  * Description    : Clear the DTOG_TX bit.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : None.
  **/
void ClearDTOG_TX(uint8_t bEpNum)
{
  _ClearDTOG_TX(bEpNum);
}

/**
  * Function Name  : SetEPAddress
  * Description    : Set the endpoint address.
  * Input          : bEpNum: Endpoint Number.
  *                  bAddr: New endpoint address.
  * Output         : None.
  * Return         : None.
  **/
void SetEPAddress(uint8_t bEpNum, uint8_t bAddr)
{
  _SetEPAddress(bEpNum, bAddr);
}

/**
  * Function Name  : GetEPAddress
  * Description    : Get the endpoint address.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Endpoint address.
  **/
uint8_t GetEPAddress(uint8_t bEpNum)
{
  return(_GetEPAddress(bEpNum));
}

/**
  * Function Name  : SetEPTxAddr
  * Description    : Set the endpoint Tx buffer address.
  * Input          : bEpNum: Endpoint Number.
  *                  wAddr: new address. 
  * Output         : None.
  * Return         : None.
  **/
void SetEPTxAddr(uint8_t bEpNum, uint16_t wAddr)
{
  _SetEPTxAddr(bEpNum, wAddr);
}

/**
  * Function Name  : SetEPRxAddr
  * Description    : Set the endpoint Rx buffer address.
  * Input          : bEpNum: Endpoint Number.
  *                  wAddr: new address.
  * Output         : None.
  * Return         : None.
**/
void SetEPRxAddr(uint8_t bEpNum, uint16_t wAddr)
{
  _SetEPRxAddr(bEpNum, wAddr);
}

/**
  * Function Name  : GetEPTxAddr
  * Description    : Returns the endpoint Tx buffer address.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Rx buffer address. 
  **/
uint16_t GetEPTxAddr(uint8_t bEpNum)
{
  return(_GetEPTxAddr(bEpNum));
}

/**
    * Function Name  : GetEPRxAddr.
    * Description    : Returns the endpoint Rx buffer address.
    * Input          : bEpNum: Endpoint Number. 
    * Output         : None.
    * Return         : Rx buffer address.
    **/
uint16_t GetEPRxAddr(uint8_t bEpNum)
{
  return(_GetEPRxAddr(bEpNum));
}

/**
  * Function Name  : SetEPTxCount.
  * Description    : Set the Tx count.
  * Input          : bEpNum: Endpoint Number.
  *                  wCount: new count value.
  * Output         : None.
  * Return         : None.
  **/
void SetEPTxCount(uint8_t bEpNum, uint16_t wCount)
{
  _SetEPTxCount(bEpNum, wCount);
}

/**
  * Function Name  : SetEPCountRxReg.
  * Description    : Set the Count Rx Register value.
  * Input          : *pdwReg: point to the register.
  *                  wCount: the new register value.
  * Output         : None.
  * Return         : None.
  **/
#if defined STM32F303xE || defined STM32F302x8 
void SetEPCountRxReg(uint16_t *pdwReg, uint16_t wCount)
{
  _SetEPCountRxReg(pdwReg, wCount);
}
#else
void SetEPCountRxReg(uint32_t *pdwReg, uint16_t wCount)
{
  _SetEPCountRxReg(dwReg, wCount);
}
#endif

/**
  * Function Name  : SetEPRxCount
  * Description    : Set the Rx count.
  * Input          : bEpNum: Endpoint Number. 
  *                  wCount: the new count value.
  * Output         : None.
  * Return         : None.
  **/
void SetEPRxCount(uint8_t bEpNum, uint16_t wCount)
{
  _SetEPRxCount(bEpNum, wCount);
}

/**
  * Function Name  : GetEPTxCount
  * Description    : Get the Tx count.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None
  * Return         : Tx count value.
  **/
uint16_t GetEPTxCount(uint8_t bEpNum)
{
  return(_GetEPTxCount(bEpNum));
}

/**
  * Function Name  : GetEPRxCount
  * Description    : Get the Rx count.
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : Rx count value.
  **/
uint16_t GetEPRxCount(uint8_t bEpNum)
{
  return(_GetEPRxCount(bEpNum));
}

/**
  * Function Name  : SetEPDblBuffAddr
  * Description    : Set the addresses of the buffer 0 and 1.
  * Input          : bEpNum: Endpoint Number.  
  *                  wBuf0Addr: new address of buffer 0. 
  *                  wBuf1Addr: new address of buffer 1.
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuffAddr(uint8_t bEpNum, uint16_t wBuf0Addr, uint16_t wBuf1Addr)
{
  _SetEPDblBuffAddr(bEpNum, wBuf0Addr, wBuf1Addr);
}

/**
  * Function Name  : SetEPDblBuf0Addr
  * Description    : Set the Buffer 1 address.
  * Input          : bEpNum: Endpoint Number
  *                  wBuf0Addr: new address.
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuf0Addr(uint8_t bEpNum, uint16_t wBuf0Addr)
{
  _SetEPDblBuf0Addr(bEpNum, wBuf0Addr);
}

/**
  * Function Name  : SetEPDblBuf1Addr
  * Description    : Set the Buffer 1 address.
  * Input          : bEpNum: Endpoint Number
  *                  wBuf1Addr: new address.
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuf1Addr(uint8_t bEpNum, uint16_t wBuf1Addr)
{
  _SetEPDblBuf1Addr(bEpNum, wBuf1Addr);
}
/**
  * Function Name  : GetEPDblBuf0Addr
  * Description    : Returns the address of the Buffer 0.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : None.
  **/
uint16_t GetEPDblBuf0Addr(uint8_t bEpNum)
{
  return(_GetEPDblBuf0Addr(bEpNum));
}

/**
  * Function Name  : GetEPDblBuf1Addr
  * Description    : Returns the address of the Buffer 1.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : Address of the Buffer 1.
  **/
uint16_t GetEPDblBuf1Addr(uint8_t bEpNum)
{
  return(_GetEPDblBuf1Addr(bEpNum));
}

/**
  * Function Name  : SetEPDblBuffCount
  * Description    : Set the number of bytes for a double Buffer 
  *                  endpoint.
  * Input          : bEpNum,bDir, wCount
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuffCount(uint8_t bEpNum, uint8_t bDir, uint16_t wCount)
{
  _SetEPDblBuffCount(bEpNum, bDir, wCount);
}

/**
  * Function Name  : SetEPDblBuf0Count
  * Description    : Set the number of bytes in the buffer 0 of a double Buffer 
  *                  endpoint.
  * Input          : bEpNum, bDir,  wCount
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuf0Count(uint8_t bEpNum, uint8_t bDir, uint16_t wCount)
{
  _SetEPDblBuf0Count(bEpNum, bDir, wCount);
}

/**
  * Function Name  : SetEPDblBuf1Count
  * Description    : Set the number of bytes in the buffer 0 of a double Buffer 
  *                  endpoint.
  * Input          : bEpNum,  bDir,  wCount
  * Output         : None.
  * Return         : None.
  **/
void SetEPDblBuf1Count(uint8_t bEpNum, uint8_t bDir, uint16_t wCount)
{
  _SetEPDblBuf1Count(bEpNum, bDir, wCount);
}

/**
  * Function Name  : GetEPDblBuf0Count
  * Description    : Returns the number of byte received in the buffer 0 of a double
  *                  Buffer endpoint.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : Endpoint Buffer 0 count
  **/
uint16_t GetEPDblBuf0Count(uint8_t bEpNum)
{
  return(_GetEPDblBuf0Count(bEpNum));
}

/**
  * Function Name  : GetEPDblBuf1Count
  * Description    : Returns the number of data received in the buffer 1 of a double
  *                  Buffer endpoint.
  * Input          : bEpNum: Endpoint Number.
  * Output         : None.
  * Return         : Endpoint Buffer 1 count.
  **/
uint16_t GetEPDblBuf1Count(uint8_t bEpNum)
{
  return(_GetEPDblBuf1Count(bEpNum));
}

/**
  * Function Name  : GetEPDblBufDir
  * Description    : gets direction of the double buffered endpoint
  * Input          : bEpNum: Endpoint Number. 
  * Output         : None.
  * Return         : EP_DBUF_OUT, EP_DBUF_IN,
  *                  EP_DBUF_ERR if the endpoint counter not yet programmed.
  **/
EP_DBUF_DIR GetEPDblBufDir(uint8_t bEpNum)
{
  if ((uint16_t)(*_pEPRxCount(bEpNum) & 0xFC00) != 0)
    return(EP_DBUF_OUT);
  else if (((uint16_t)(*_pEPTxCount(bEpNum)) & 0x03FF) != 0)
    return(EP_DBUF_IN);
  else
    return(EP_DBUF_ERR);
}

/**
  * Function Name  : FreeUserBuffer
  * Description    : free buffer used from the application realizing it to the line
                   toggles bit SW_BUF in the double buffered endpoint register
  * Input          : bEpNum, bDir
  * Output         : None.
  * Return         : None.
  **/
void FreeUserBuffer(uint8_t bEpNum, uint8_t bDir)
{
  if (bDir == EP_DBUF_OUT)
  { /* OUT double buffered endpoint */
    _ToggleDTOG_TX(bEpNum);
  }
  else if (bDir == EP_DBUF_IN)
  { /* IN double buffered endpoint */
    _ToggleDTOG_RX(bEpNum);
  }
}

/**
  * Function Name  : ToWord
  * Description    : merge two byte in a word.
  * Input          : bh: byte high, bl: bytes low.
  * Output         : None.
  * Return         : resulted word.
  **/
uint16_t ToWord(uint8_t bh, uint8_t bl)
{
  uint16_t wRet;
  wRet = (uint16_t)bl | ((uint16_t)bh << 8);
  return(wRet);
}

/**
  * Function Name  : ByteSwap
  * Description    : Swap two byte in a word.
  * Input          : wSwW: word to Swap.
  * Output         : None.
  * Return         : resulted word.
  **/
uint16_t ByteSwap(uint16_t wSwW)
{
  uint8_t bTemp;
  uint16_t wRet;
  bTemp = (uint8_t)(wSwW & 0xff);
  wRet =  (wSwW >> 8) | ((uint16_t)bTemp << 8);
  return(wRet);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
