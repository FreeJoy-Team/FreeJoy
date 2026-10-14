#ifndef BCD_TEST_PLATFORM_H
#define BCD_TEST_PLATFORM_H

/* Replace only hardware access; compile the real button/configuration code. */
#define __PERIPHERY_H__
#define __AXES_TO_BUTTON_H__
#include "common_types.h"

typedef struct { uint16_t IDR; } GPIO_TypeDef;
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t number;
} pin_config_t;
enum { Bit_RESET, Bit_SET };
extern pin_config_t pin_config[USED_PINS_NUM];
int64_t GetMillis(void);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *port, uint16_t pin);
void GPIO_WriteBit(GPIO_TypeDef *port, uint16_t pin, int value);
void AxisToButtonsGet(uint8_t *buf, dev_config_t *config, uint8_t *pos);

#endif
