# FreeJoy

<img src="https://github.com/FreeJoy-Team/FreeJoy/blob/master/images/main.png">

FreeJoy is a widely configurable game device controller based on the cheap STM32F103C8 microcontroller board. It allows you to create your own HOTAS, pedals, steering wheel device, etc, or customize a purchased one.

## Getting started

See [our wiki](https://github.com/FreeJoy-Team/FreeJoyWiki) for instructions on how to flash firmware to your board and how to configure the device for your application.

## Features
FreeJoy supports the following external periphery:

- 8 analog inputs (12 bit output resolution)
- axis to buttons function (up to 12 buttons per axis)
- buttons/encoders to axis functions
- 128 digital inputs (buttons, toggle switches, hat povs, encoders)
- 5 shift modifiers
- 4 hat povs
- 16 incremental encoders
- shift registers 74HC165 and CD4021
- digital sensors  TLE5010/5011, TLE5012B, AS5048A, AS5600, MLX90393 (SPI interface only)
- external ADCs ADS1115 and MCP3201/02/04/08
- 4 PWM channels for lighting
- 24 LEDs (single or matrix) bindable to buttons' states or controlled by host software (see [Host-controlled LEDs](#host-controlled-leds) section)
- 50 addressable LEDs ws2812b or pl9823 with effects and the ability to control via SimHub;
- device name and other USB settings

## Axes
FreeJoy supports up to 8 analog inputs at pins A0-A7 and digital sensors as axis sources. Every axis has its own settings, including:

* Source/destination (X, Y, Z, Rx, Ry, Rz, Slider1, Slider2)
* Output enabling/disabling
* Resolution
* Calibration (manual or auto)
* Smoothing (7 levels of filtering)
* Inversion
* Deadband (dynamic or center)
* Axis offset option (magnet offset)
* Curve shaping
* Functions for combined axes
* Buttons from axes
* Axes from buttons/encoders

## Buttons
FreeJoy allows you to connect up to 128 digital inputs as single inputs (tied to VCC or GND), button matrices, shift register inputs and axis-to-buttons inputs. Inputs can be configured as:

* Regular push button
* Inverted push button
* Toggle switch ON/OFF
* Toggle switch ON
* Toggle switch OFF
* POV hat button
* Incremental encoder input
* Radio buttons
* Sequential buttons
* 5 shifts

## FreeJoy Configurator utility
FreeJoy configuration is possible using an external [utility](https://github.com/FreeJoy-Team/FreeJoyConfiguratorQt).

<img src="https://github.com/FreeJoy-Team/FreeJoyWiki/blob/master/images/main.jpg" width="800"/>

## Host-controlled LEDs
FreeJoy supports controlling up to 24 LEDs from host software using HID reports. To use this feature:
1. In FreeJoy Configurator, set the LED source to **External**.
2. Send a HID Output Report to the second HID device (Usage Page: 0xFF00, Interface: 1) with the following format:

| Byte | Value | Description |
| --- | --- | --- |
| 0 | 6 | Report ID |
| 1-4 | Bitmask | LED states (1 bit per LED, 4 bytes, little-endian) |

Example: to turn on LEDs 1 and 3, send `06 05 00 00 00`.

### Python Example
You can use the `hidapi` library to control LEDs from Python:

```python
import hid

# FreeJoy USB VID/PID
VID = 0x0483
PID = 0x5750

def set_freejoy_leds(led_mask):
    # Find the correct interface (Interface 1 for LEDs)
    target_path = None
    for device in hid.enumerate(VID, PID):
        if device['interface_number'] == 1:
            target_path = device['path']
            break

    if not target_path:
        print("FreeJoy interface 1 not found")
        return

    try:
        device = hid.device()
        device.open_path(target_path)
        
        # Report ID 6 + 4 bytes of bitmask (little-endian)
        report = [0x06] + list(led_mask.to_bytes(4, 'little'))
        device.write(report)
        device.close()
    except Exception as e:
        print(f"Error: {e}")

# Example: Turn on LEDs 1 and 3 (bitmask 0x05)
set_freejoy_leds(0x00000005)
```
