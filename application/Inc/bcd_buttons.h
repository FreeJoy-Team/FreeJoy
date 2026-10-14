/** Binary-coded switch pin encoding, shared with the configurator. */
#ifndef __BCD_BUTTONS_H__
#define __BCD_BUTTONS_H__

#include "common_types.h"

#define BCD_SWITCH_COUNT 4
#define BCD_BITS_PER_SWITCH 4
#define BCD_PIN_TYPES_PER_SWITCH (2 * BCD_BITS_PER_SWITCH)

static inline uint8_t BcdPinIsInput(pin_t type)
{
    return type >= BCD1_BIT0_GND && type <= BCD4_BIT3_VCC;
}

static inline uint8_t BcdPinIsVcc(pin_t type)
{
    return ((type - BCD1_BIT0_GND) % BCD_PIN_TYPES_PER_SWITCH) >= BCD_BITS_PER_SWITCH;
}

static inline uint8_t BcdPinGroup(pin_t type)
{
    return (type - BCD1_BIT0_GND) / BCD_PIN_TYPES_PER_SWITCH;
}

static inline uint8_t BcdPinBit(pin_t type)
{
    return (type - BCD1_BIT0_GND) % BCD_BITS_PER_SWITCH;
}

#endif
