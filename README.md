# LEDMatrixDriver

A replacement for Arduino's LedControl library.

## Features
- Can drive more than 8 segments (limited only by memory)
- Writing to the display is controlled by software
- Much faster because it uses hardware [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface) with soft SS
- Can use an external memory or self-allocated buffer
- (Optional) support for Adafruit_GFX library `-DUSE_ADAFTUIT_GFX to enable`

## Pin selection
Each segment of the display requires 16 bits and can all be shifted in at once.  To make transfers faster we use hardware SPI instead of software.

The pins are therefore fixed:
- MOSI is connected to DIN,
- SCK is connected to CLK,
- MISO can't be used (unless by other SPI device),
- SS can't be used (we're not using the hardware pin).

The user has to select which pin to use for software SS but **don't** use any of the hardware SPI pins. Please check pin numbers specific for your [hardware platform](https://www.arduino.cc/en/Reference/SPI).

Recommended pins for Arduino Uno:
- DIN is connected to 11
- CLK is connected to 13
- CS is connected to 9

## Contributors
- Marko Oette - patches for Arduino AVR platform, example sketch
- Søren Thing Andersen - support for 7-seg displays with examples
- Scott Baker - examples clean-up
- James Gohl - module reversal concept and some code, another example
