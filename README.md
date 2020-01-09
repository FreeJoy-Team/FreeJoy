# FreeJoy
FreeJoy implements USB HID joystick device with flexible configuration. It is based on STM32F103C8 microcontroller and supports up to 8 analog axis, 128 digital inputs (buttons, toggle switches, etc.) and 4 POVs. It is targeting a cheap BluePill board based on 32-bit STM32F103 microcontroller.

<img src="https://d.radikal.ru/d33/1911/e8/a8ff2119cfc7.jpg" width="250">

## Flexibility
FreeJoy supports the following external periphery:

* Analog inputs
* Push buttons
* Toggle switches
* POV hats
* Incremental encoders
* Converting analog inputs to buttons (up to 10 buttons per axis)

## Analog inputs
FreeJoy supports up to 8 analog inputs at pins A0-A7. Every axis have its settings as:

* Calibration (manual or automatical)
* Smothing (4 levels of filtration)
* Invertion
* Curve shaping

## Buttons
FreeJoy allows you to connect up to 128 digital inputs as single inputs (tied to VCC or GND) and matrix of buttons. Each input can be configured as:

* Regular push button
* Inverted push button
* Toggle switch ON/OFF
* Toggle switch ON
* Toggle switch OFF
* POV hat button
* Incremental encoder input

## Exrenal configuration tool 
FreeJoy configuration is possible both by changing definitions in source code and by using external [utility](https://github.com/vostrenkov/FreeJoyConfigurator).

For information how to configure your device check [wiki page](https://github.com/vostrenkov/FreeJoy/wiki)
