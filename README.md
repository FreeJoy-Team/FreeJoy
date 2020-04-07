# FreeJoy

<img src="https://github.com/FreeJoy-Team/FreeJoy/blob/master/images/main.png">

FreeJoy is widely configurable game device controller based on cheap STM32F103C8 microcontroller board. It allows you to create your own HOTAS, pedals, steering wheel device or customize a purchased one.

## Getting started

Welcome to wiki pages of FreeJoy and FreeJoy-Configurator projects to get instuctions [how to flash firmware to your board](https://github.com/vostrenkov/FreeJoy/wiki) and [how to configure device for your application](https://github.com/FreeJoy-Team/FreeJoyConfigurator/wiki).

## Features
FreeJoy supports the following external periphery:

* 8 analog inputs
* 128 push buttons or toggle switches
* 4 POV hats
* 16 Incremental encoders
* Converting analog inputs to buttons (up to 12 buttons per axis)
* Shift registers 74HC165 and CD4021 for extending inpits number
* TLE5010/TLE5011 sensors as axes sources
* 3 PWM channels for LED lighting
* LEDs bindable to buttons states

## Axes
FreeJoy supports up to 8 analog inputs at pins A0-A7 and TLE5010/5011 sensors as axes sources. Every axis has its own settings as:

* Source/destination (X, Y, Z, Rx, Ry, Rz, Slider1, Slider2)
* Output enabling/disabling
* Resolution
* Calibration (manual or auto)
* Smoothing (7 levels of filtration)
* Inversion
* Deadband (dynamic or center)
* Axis offset option (magnet offset)
* Curve shaping
* Functions for combined axes
* Axes from buttons/encoders

## Buttons
FreeJoy allows you to connect up to 128 digital inputs as single inputs (tied to VCC or GND), matrix of buttons, shift register inputs and axes to buttons inputs. Inputs can be configured as:

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
FreeJoy configuration is possible using external [utility](https://github.com/vostrenkov/FreeJoyConfigurator).

<img src="https://github.com/FreeJoy-Team/FreeJoy/blob/master/images/configurator.png" width="600"/>

