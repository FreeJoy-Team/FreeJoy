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
- 24 LEDs (single or matrix) bindable to buttons' states
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

