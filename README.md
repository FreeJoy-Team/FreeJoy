# FreeJoy
FreeJoy implements USB HID joystick device with flexible configuration. It is based on STM32F103C8 microcontroller and supports up to 8 analog axes and 12 digital inputs (buttons).

## Flexibility
FreeJoy allows to select the configuration of joystick device you exactly need. You can choose any of inputs shown below:

* Up to 3-axis joystick axes (X, Y and Z)
* Up to 3 linear axes 
* Throttle axis
* Rudder axis
* Up to 12 digital inputs (such as push-buttons)

## Default pinout
![Alt text](https://a.radikal.ru/a29/1807/3b/911b58654ab7.jpg)

Analog channes (axes) are hardwired to A0-A7 pin due to STM32 peripheral architecture, but digital pin (buttons) can be remapped if needed. For remapping button pins change corresponding definitions in main.h file.

## Axis priority
When you select to use axes, they are mapped to analog channels (A0-A7) in according to axis priority. Axis with higher priority is mapped to channel with lower number. Fow example if the highest priority Throttle axis is enabled it is mapped to A0 channel. If this axis is disabled then the highest priority enabled axis is mapped to A0.  

Axis priotiries from highest to lowest:
- Throttle axis
- Rudder axis
- Joystick X-axis
- Joystick Y-axis
- Joystick Z-axis
- Secondary linear axis 1 (Rx)
- Secondary linear axis 2 (Ry)
- Secondary linear axis 3 (Rz)

### Notes
You cannot map Joystick Y-axis without using Joystick X-axis. If you want to use the only axis in Joystick or Linear Axis groupe it will always be X-axis or Rx-axis respectively. Z and Rz axes cannot be mapped without X, Y and Rx, Ry too.

