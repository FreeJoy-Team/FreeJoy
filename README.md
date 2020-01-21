# FreeJoy
FreeJoy implements USB HID joystick device with flexible configuration. It is based on STM32F103C8 microcontroller and supports up to 8 analog axis, 128 digital inputs (buttons, toggle switches, etc.), 4 POVs, shift registers, TLE5010/5011 sensors and axes to buttons feature. It is targeting a cheap BluePill board based on 32-bit STM32F103 microcontroller.

<img src="https://d.radikal.ru/d33/1911/e8/a8ff2119cfc7.jpg" width="250">

## Flexibility
FreeJoy supports the following external periphery:

* Analog inputs
* Push buttons
* Toggle switches
* POV hats
* Incremental encoders
* Converting analog inputs to buttons (up to 10 buttons per axis)
* Shift registers 74HC165 and CD4021
* TLE5010/TLE5011 sensors

## Analog inputs
FreeJoy supports up to 8 analog inputs at pins A0-A7. Every axis have its settings as:

* Output enabling/disabling
* Resolution
* Calibration (manual or automatical)
* Smothing (4 levels of filtration)
* Invertion
* Magnet offset option
* Curve shaping

## Axes to buttons

Each axis can be used as source for convertion axis output to buttons signals. Up to 12 buttons per axis is supported.

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

<img src="https://b.radikal.ru/b12/2001/56/821d02d9e447.png" width="500"/>

For information how to configure your device check [wiki page](https://github.com/vostrenkov/FreeJoy/wiki)
