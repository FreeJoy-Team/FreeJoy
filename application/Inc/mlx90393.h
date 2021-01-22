/**
  ******************************************************************************
  * @file           : mlx90393.h
  * @brief          : Header for mlx90393.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MLX90393_H__
#define __MLX90393_H__

#include "common_types.h"
#include "periphery.h"

#define MLX90393_SPI_MODE							3

#define MLX_SPI												0
#define MLX_I2C												1

// Commands
#define	MLX_START_BURST 							0x10
#define	MLX_START_WAKE_ON_CHANGE 			0x20
#define	MLX_START_SINGLE							0x30
#define	MLX_EXIT 											0x80
#define	MLX_MEM_RECALL 								0xD0
#define	MLX_MEM_STORE 								0xE0
#define	MLX_RESET 										0xF0

// Data
#define MLX_Z													0x08
#define MLX_Y													0x04
#define MLX_X													0x02
#define MLX_T													0x01

// Reg0 bits
#define BIST													0x100
#define Z_SERIES											0x80
#define GAIN_SEL(val) 								((val << 4) & 0x70) 
#define HAL_CONF(val) 								((val) & 0x0F) 

// Reg1 bits
#define TRIG_INT 											0x8000
#define SPI_MODE 											0x4000
#define I2C_MODE 											0x2000
#define WOC_DIFF											0x1000
#define EXT_TRIG											0x800
#define TCMP_EN												0x400
#define BURST_SEL_Z 									0x200
#define BURST_SEL_Y 									0x100
#define BURST_SEL_X 									0x80
#define BURST_SEL_T 									0x40
#define BURST_DR(val)									((val) & 0x3F)

// Reg2 bits
#define OSR2(val)											(((val_ << 11) & 0x1800)
#define RES(z,y,x)  									(((z) << 9 | (y) << 7 | (x) << 5) & 0x7E0)
#define DIG_FILT(val) 								(((val) << 2) & 0x1C)
#define OSR(val)   										((val) & 0x03)

void MLX90393_Start(uint8_t mode, sensor_t * sensor);

int MLX90393_GetData(uint16_t * data, sensor_t * sensor, uint8_t channel);

void MLX90393_StartDMA(uint8_t mode, sensor_t * sensor);
void MLX90393_StopDMA(sensor_t * sensor);

#endif 	/* __MLX90393_H__ */

