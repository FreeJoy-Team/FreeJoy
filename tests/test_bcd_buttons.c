#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "buttons.h"
#include "bcd_buttons.h"

_Static_assert(BUTTON_GND == 1 && BUTTON_VCC == 2 && UART_TX == 31, "legacy pin IDs");
_Static_assert(BCD1_BIT0_GND == 32 && BCD4_BIT3_VCC == 63, "BCD pin IDs");
_Static_assert(sizeof(pin_t) == 1, "wire format");

pin_config_t pin_config[USED_PINS_NUM];
static GPIO_TypeDef ports[USED_PINS_NUM];
static uint32_t now;
static uint8_t shift_count;
static uint8_t axis_count;
static dev_config_t config;
extern physical_buttons_state_t physical_buttons_state[MAX_BUTTONS_NUM];

int64_t GetMillis(void) { return now; }
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *port, uint16_t pin) { return !!(port->IDR & pin); }
void GPIO_WriteBit(GPIO_TypeDef *port, uint16_t pin, int value)
{ (void)port; (void)pin; (void)value; }
void ShiftRegistersGet(uint8_t *buf, dev_config_t *cfg, uint8_t *pos)
{
    (void)cfg;
    for (unsigned i = 0; i < shift_count && *pos < MAX_BUTTONS_NUM; i++)
        buf[(*pos)++] = 0;
}
void AxisToButtonsGet(uint8_t *buf, dev_config_t *cfg, uint8_t *pos)
{
    (void)cfg;
    for (unsigned i = 0; i < axis_count && *pos < MAX_BUTTONS_NUM; i++)
        buf[(*pos)++] = 0;
}

static void reset(void)
{
    memset(&config, 0, sizeof(config));
    memset(ports, 0, sizeof(ports));
    memset(raw_buttons_data, 0, sizeof(raw_buttons_data));
    memset(physical_buttons_state, 0, sizeof(physical_buttons_state_t) * MAX_BUTTONS_NUM);
    memset(logical_buttons_state, 0, sizeof(logical_buttons_state_t) * MAX_BUTTONS_NUM);
    for (unsigned i = 0; i < USED_PINS_NUM; i++)
        pin_config[i] = (pin_config_t){&ports[i], 1, i};
    for (unsigned i = 0; i < MAX_BUTTONS_NUM; i++)
        config.buttons[i].physical_num = -1;
    for (unsigned i = 0; i < 5; i++) config.shift_config[i].button = -1;
    shift_count = axis_count = 0;
    now = 100;
    /* An empty scan resets all group debounce state. */
    assert(ButtonsReadPhysical(&config, raw_buttons_data) == 0);
    ButtonsDebounceProcess(&config);
}

static void group(unsigned group, unsigned bits, unsigned pin_start, unsigned vcc_mask)
{
    for (unsigned bit = 0; bit < bits; bit++)
        config.pins[pin_start + bit] = BCD1_BIT0_GND + group * 8 + bit +
                                     ((vcc_mask & (1u << bit)) ? 4 : 0);
}

static void value(unsigned group, unsigned code)
{
    for (unsigned pin = 0; pin < USED_PINS_NUM; pin++)
    {
        pin_t type = config.pins[pin];
        if (BcdPinIsInput(type) && BcdPinGroup(type) == group)
        {
            unsigned active = !!(code & (1u << BcdPinBit(type)));
            ports[pin].IDR = BcdPinIsVcc(type) ? active : !active;
        }
    }
}

static unsigned scan(void)
{
    unsigned count = ButtonsReadPhysical(&config, raw_buttons_data);
    ButtonsDebounceProcess(&config);
    return count;
}

static void expect(unsigned first, unsigned count, int code)
{
    for (unsigned i = 0; i < count; i++)
        assert(physical_buttons_state[first + i].current_state == ((int)i == code));
}

static void all_codes_and_polarities(void)
{
    for (unsigned g = 0; g < 4; g++)
        for (unsigned bits = 1; bits <= 4; bits++)
            for (unsigned polarity = 0; polarity < (1u << bits); polarity++)
            {
                reset();
                group(g, bits, 5, polarity);
                for (unsigned code = 0; code < (1u << bits); code++)
                {
                    value(g, code);
                    assert(scan() == (1u << bits));
                    expect(0, 1u << bits, code);
                }
            }
}

static void independent_groups_and_order(void)
{
    reset();
    config.pins[0] = BUTTON_VCC;
    ports[0].IDR = 1;
    config.pins[1] = BUTTON_GND;
    ports[1].IDR = 0;
    shift_count = 3;
    axis_count = 2;
    config.pins[25] = BUTTON_ROW;
    config.pins[26] = BUTTON_COLUMN;
    /* Reverse physical pin order must not change group/bit order. */
    for (unsigned g = 0; g < 4; g++)
        group(g, 4, 20 - g * 4, g + 1);
    pin_t swap = config.pins[20];
    config.pins[20] = config.pins[23];
    config.pins[23] = swap;
    for (unsigned code = 0; code < 16; code++)
    {
        for (unsigned g = 0; g < 4; g++) value(g, (code + g * 3) % 16);
        assert(scan() == 72);
        assert(raw_buttons_data[0] == 1); /* Matrix precedes registers/axes/direct pins. */
        assert(raw_buttons_data[6] == 1 && raw_buttons_data[7] == 1);
        for (unsigned g = 0; g < 4; g++) expect(8 + 16 * g, 16, (code + g * 3) % 16);
    }
}

static void independent_debounce(void)
{
    reset();
    config.button_debounce_ms = 5;
    group(0, 4, 0, 0);
    group(1, 2, 4, 3);
    value(0, 1); value(1, 0); scan();
    now = 105; scan(); expect(0, 16, 1); expect(16, 4, 0);
    now = 110; value(0, 3); value(1, 2); scan();
    now = 114; value(0, 7); scan();
    now = 115; scan(); expect(0, 16, 1); expect(16, 4, 2);
    now = 119; scan(); expect(0, 16, 7); expect(16, 4, 2);
}

static void debounce_and_hid(void)
{
    reset();
    config.button_debounce_ms = 5;
    group(0, 4, 0, 5);
    for (unsigned i = 0; i < 16; i++) config.buttons[i].physical_num = i;
    value(0, 0);
    scan(); expect(0, 16, -1);
    now = 104; scan(); expect(0, 16, -1);
    now = 105; scan(); expect(0, 16, 0);
    /* 7 -> 8 may pass through 15: restart debounce for every changed code. */
    value(0, 7); now = 110; scan(); expect(0, 16, 0);
    value(0, 15); now = 114; scan(); expect(0, 16, 0);
    value(0, 8); now = 118; scan(); expect(0, 16, 0);
    now = 122; scan(); expect(0, 16, 0);
    now = 123; scan(); expect(0, 16, 8);
    ButtonsReadLogical(&config);
    now++; /* The existing logical-button engine processes edges on the next pass. */
    ButtonsReadLogical(&config);
    uint8_t hid[MAX_BUTTONS_NUM / 8] = {0};
    ButtonsGet(hid, NULL, NULL, NULL);
    assert(hid[0] == 0 && hid[1] == 1);
    for (unsigned i = 2; i < sizeof(hid); i++) assert(hid[i] == 0);
    /* Unsigned elapsed time survives a 32-bit millisecond rollover. */
    value(0, 4); now = UINT32_MAX - 2; scan(); expect(0, 16, 8);
    now = 1; scan(); expect(0, 16, 8);
    now = 2; scan(); expect(0, 16, 4);
}

static void invalid_and_capacity(void)
{
    reset();
    group(0, 4, 0, 15);
    config.pins[1] = NOT_USED; /* Missing bit 1. */
    value(0, 3); assert(scan() == 16); expect(0, 16, -1);
    config.pins[1] = BCD1_BIT1_VCC;
    config.pins[4] = BCD1_BIT0_GND; /* Duplicate even across polarities. */
    value(0, 3); assert(scan() == 16); expect(0, 16, -1);
    group(1, 2, 5, 3); value(1, 2);
    assert(scan() == 20); expect(0, 16, -1); expect(16, 4, 2);
    reset();
    group(0, 4, 0, 0);
    shift_count = 112;
    value(0, 15); assert(scan() == 128); expect(112, 16, 15);
    /* Capacity overflow must not silently expose a subset of positions. */
    shift_count = 120;
    uint8_t guarded[MAX_BUTTONS_NUM + 2];
    memset(guarded, 0xA5, sizeof(guarded));
    assert(ButtonsReadPhysical(&config, guarded + 1) == 128);
    assert(guarded[0] == 0xA5 && guarded[MAX_BUTTONS_NUM + 1] == 0xA5);
    memcpy(raw_buttons_data, guarded + 1, MAX_BUTTONS_NUM);
    ButtonsDebounceProcess(&config);
    expect(120, 8, -1);
    /* Legacy-only setup still allocates ordinary pins without extra buttons. */
    reset();
    config.pins[0] = BUTTON_GND;
    config.pins[1] = BUTTON_VCC;
    ports[1].IDR = 1;
    assert(scan() == 2);
    now++; scan();
    assert(physical_buttons_state[0].current_state == 1);
    assert(physical_buttons_state[1].current_state == 1);
}

int main(void)
{
    all_codes_and_polarities();
    puts("PASS: every code for 1-4 bits, all four groups and all polarity masks");
    independent_groups_and_order();
    independent_debounce();
    puts("PASS: simultaneous independent groups and legacy input ordering");
    debounce_and_hid();
    puts("PASS: group debounce, transient suppression, HID output and timer rollover");
    invalid_and_capacity();
    puts("PASS: invalid configurations, 128-button capacity and buffer bounds");
    return 0;
}
