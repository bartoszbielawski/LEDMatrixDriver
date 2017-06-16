# LEDMatrixDriver

A replacement for Arduino's LedControl library.

Features:
- Can driver more than 8 segments (limited only by the memory),
- Writing to the display is controlled by the software,
- Much, much faster (as it uses hardware SPI with soft SS)
- Can use an external memory or self-allocated buffer

## Contributors
- Marko Oette	 - patches for Arduino AVR platform, example sketch
