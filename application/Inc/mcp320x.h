/**
  ******************************************************************************
  * @file           : mcp320x.h
  * @brief          : Header for mcp320x.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MCP320X_H__
#define __MCP320X_H__

#include "common_types.h"
#include "periphery.h"

#define MCP32xx_SPI_MODE						1

uint16_t MCP320x_GetData(sensor_t * sensor, uint8_t channel);
void MCP320x_StartDMA(sensor_t * sensor, uint8_t channel);
void MCP320x_StopDMA(sensor_t * sensor);

#endif 	/* __MCP320X_H__ */

