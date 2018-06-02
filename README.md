# LEDMatrixDriver

A replacement for Arduino's LedControl library.

## Features
- Can driver more than 8 segments (limited only by the memory),
- Writing to the display is controlled by the software,
- Much, much faster (as it uses hardware SPI with soft SS)
- Can use an external memory or self-allocated buffer
- (Optional) support for Adafruit_GFX library - use -DUSE_ADAFTUIT_GFX to enable.

## Pin selection
Each segment of the display needs 16 bits and they can all be shifted in at once. 
To make transfers much faster I have decided to use hardware SPI. 
The pins are therefore fixed:
- MOSI is connected to DIN,
- SCK is connected to CLK,
- MISO can't be used (unless by other SPI device),
- SS can't be used (we're not using the hardware pin).

The user has to select which pin to use for software SS but *don't use any of the hardware SPI pins*. Please check pin numbers specific for your hardware platform. 

## Contributors
- Marko Oette	 - patches for Arduino AVR platform, example sketch
- Søren Thing Andersen - support for 7-seg displays with examples
