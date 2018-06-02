/*
 * LEDMatrixDriver.cpp
 *
 *  Created on: 30.03.2017
 *      Author: Bartosz Bielawski
 */

#include "LEDMatrixDriver.hpp"
#include <Arduino.h>

LEDMatrixDriver::LEDMatrixDriver(uint8_t N, uint8_t ssPin, uint8_t* frameBuffer_):
#ifdef USE_ADAFRUIT_GFX
	Adafruit_GFX(N*8, N),
#endif
	N(N),
	spiSettings(5000000, MSBFIRST, SPI_MODE0),
	frameBuffer(frameBuffer_),
	selfAllocated(frameBuffer_ == nullptr),
	ssPin(ssPin)
{
	if (selfAllocated)
		frameBuffer = new uint8_t[N*8];

	clear();	// initally clear the buffer as the memory will not be initialized on reset (old content will be in memory yet)

	pinMode(ssPin, OUTPUT);
	digitalWrite(ssPin, 1);
	SPI.begin();
#ifdef ESP8266
	SPI.setHwCs(false);
#endif

	setEnabled(false);
	setIntensity(0);
	_sendCommand(LEDMatrixDriver::TEST);			//no test
	_sendCommand(LEDMatrixDriver::DECODE);			//no decode
	_sendCommand(LEDMatrixDriver::SCAN_LIMIT | 7);	//all lines
}

LEDMatrixDriver::~LEDMatrixDriver()
{
	if (selfAllocated)
		delete[] frameBuffer;
}

void LEDMatrixDriver::setPixel(uint16_t x, uint16_t y, bool enabled)
{
	uint8_t* p = _getBufferPtr(x,y);
	if (!p)
		return;

	uint16_t b = 7 - (x & 7);			//bit

	if (enabled)
		*p |=  (1<<b);
	else
		*p &= ~(1<<b);
}

bool LEDMatrixDriver::getPixel(uint16_t x, uint16_t y) const
{
	uint8_t* p = _getBufferPtr(x,y);
	if (!p)
		return false;

	uint16_t b = 7 - (x & 7);			//bit

	return *p & (1 << b);
}

void LEDMatrixDriver::setColumn(uint16_t x, uint8_t value)
{
	if (x >= (8*N))
		return;

	for (uint8_t y = 0; y < 8; ++y)
	{
		setPixel(x, y, value & 1);
		value >>= 1;
	}
}

void LEDMatrixDriver::setEnabled(bool enabled)
{
	_sendCommand(ENABLE | (enabled ? 1: 0));
}

void LEDMatrixDriver::setIntensity(uint8_t level)
{
	_sendCommand(INTENSITY | level);
}

/**
 * Scan only digits 0..level.
 *
 * Beware: For level <=2 you must make sure that R_SET is appropriate
 * so the Maximum Segment Current is not higher than these numbers:
 * 		level	Max Current
 *		  0			10mA
 *		  1			20mA
 *		  2			30mA
 *
 * The command is sent to all drivers.
 */
void LEDMatrixDriver::setScanLimit(uint8_t level)
{
	_sendCommand(SCAN_LIMIT | level);
}

/**
 * Sets the Decode-Mode Register.
 *
 * Each set bit in mask enables "BCD Code B" decoding for a digit.
 * When "BCD Code B" is enabled the 4 lower bits of a digit are parsed:
 *    0-9  => The same digit is rendered
 *    10   => A dash (-) is rendered.
 *    11   => The letter "E" is rendered.
 *    12   => The letter "H" is rendered.
 *    13   => The letter "L" is rendered.
 *    14   => The letter "P" is rendered.
 *    15   => Blank display
 * Bit 7 (0x80) still controls the decimal point (dot).
 *
 * The command is sent to all drivers.
 */
void LEDMatrixDriver::setDecode(uint8_t mask)
{
	_sendCommand(DECODE | mask);
}

/**
 * Sets a digit to the specified value.
 * Digits are numbered 0..(8*N-1).
 * Digits 0..7 are in the first controller, digits 8..15 in the next, etc.
 */
void LEDMatrixDriver::setDigit(uint16_t digit, uint8_t value, bool dot)
{
	if (digit >= (8*N))
		return;

	// The frameBuffer is organized as 8 rows of N bytes.
	uint8_t row = digit % 8;
	uint8_t controller = digit / 8;

	frameBuffer[row * N + controller] = value | (dot ? 1<<7 : 0);
}

void LEDMatrixDriver::_sendCommand(uint16_t command)
{
	SPI.beginTransaction(spiSettings);
	digitalWrite(ssPin, 0);
	for (uint8_t i = 0; i < N; ++i)
	{
		SPI.transfer16(command);
	}
	digitalWrite(ssPin, 1);
	SPI.endTransaction();
}

void LEDMatrixDriver::_displayRow(uint8_t row)
{
	SPI.beginTransaction(spiSettings);
	digitalWrite(ssPin, 0);
	for (uint16_t d = 0; d < N; d++)
	{
		uint16_t cmd = ((row + 1) << 8) | frameBuffer[d + row*N];
		SPI.transfer16(cmd);
	}
	digitalWrite(ssPin, 1);
	SPI.endTransaction();
}

uint8_t* LEDMatrixDriver::_getBufferPtr(uint16_t x, uint16_t y) const
{
	if (y >= 8)
		return nullptr;
	if (x >= (8*N))
		return nullptr;

	uint16_t B = x >> 3;		//byte

	return frameBuffer + y*N + B;
}

void LEDMatrixDriver::display()
{
	for (uint8_t y = 0; y < 8; y++)
	{
		_displayRow(y);
	}
}

void LEDMatrixDriver::scroll( scrollDirection direction )
{
	int cnt = 0;
	switch( direction )
	{
		case scrollDirection::scrollUp:
			cnt = 7*(N);	// moving 7 rows of N segments
			memmove(frameBuffer, frameBuffer + N, cnt);
			memset(frameBuffer+cnt, 0, N);		// Clear last row
			break;
			
		case scrollDirection::scrollDown:
			cnt = 7*N; // moving 7 rows of N segments
			memmove(frameBuffer+N, frameBuffer, cnt);
			memset(frameBuffer, 0, N);		// Clear first row
			break;
			
		case scrollDirection::scrollRight:
			// Scrolling right needs to be done by bit shifting every uint8_t in the frame buffer
			// Bits that overlap need to be carried to the next cell in a row
			for( int i = 8*N; i >= 0; i-- )
			{
				uint8_t n = frameBuffer[i] & 1;
				frameBuffer[i] >>= 1;
				if( (i+1)%N > 0 )
					frameBuffer[i+1] |= n<<7;
			}
			break;
			
		case scrollDirection::scrollLeft:
			// Scrolling left needs to be done by bit shifting every uint8_t in the frame buffer
			// Bits that overlap need to be carried to the prev cell in a row
			for( int i = 0; i < 8*N; i++ )
			{
				uint8_t n = frameBuffer[i] & B10000000;
				frameBuffer[i] <<= 1;
				if( i%N > 0 )
					frameBuffer[i-1] |= n>>7;
			}
			break;
	}
}
