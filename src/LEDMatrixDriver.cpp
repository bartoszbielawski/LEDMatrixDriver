/*
 * LEDMatrixDriver.cpp
 *
 *  Created on: 30.03.2017
 *      Author: Bartosz Bielawski
 */

#include "LEDMatrixDriver.hpp"
#include <Arduino.h>

LEDMatrixDriver::LEDMatrixDriver(uint8_t N, uint8_t ssPin, uint8_t* frameBuffer_):
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
	if (y >= 8)
		return;
	if (x >= (8*N))
		return;

	uint16_t B = x >> 3;		//byte
	uint16_t b = 7 - (x & 7);			//bit

	uint8_t& v = frameBuffer[y*N + B];

	if (enabled)
		v |=  (1<<b);
	else
		v &= ~(1<<b);
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
	uint8_t* buf = NULL;
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

void LEDMatrixDriver::setPixel(uint8_t n, uint8_t x, uint8_t y, bool enabled)
{
	if (y >= 8 || x >= 8 || n < 0 || n >= N)
		return;
	setPixel(x + 8 * n, y, enabled);
}

void LEDMatrixDriver::clear(uint8_t n)
{
	if (n < 0 || n >= N)
		return;
	for (uint8_t y = 0; y < 8; ++y) {
		setRow(n, y, 0);
	}
}

void LEDMatrixDriver::setColumn(uint8_t n, uint8_t x, uint8_t value)
{
	if (x >= 8 * N || n < 0 || n >= N)
		return;
	setColumn( x + 8 * n , value);
}

void LEDMatrixDriver::setRow(uint8_t n, uint8_t y, uint8_t value)
{
	if (y >= 8 || n < 0 || n >= N)
		return;
	frameBuffer[N * y + n] = value;
}

void LEDMatrixDriver::setRepeatRow(uint8_t y, uint8_t value)
{
	if (y >= 8)
		return;
	for (uint8_t n = 0; n < N; ++n)
	{
		setRow(n, y, value);
	}
}

void LEDMatrixDriver::setRepeatRow16(uint8_t y, uint16_t value)
{
	if (y >= 8 || N < 2)
		return;
	uint8_t high = value >> 8;
	for (uint8_t n = 0; n < N; ++n)
	{
		setRow(n++, y, high);
		setRow(n, y, value);
	}
}

void LEDMatrixDriver::full(uint8_t n)
{
	if (n < 0 || n >= N)
		return;
	for (uint8_t y = 0; y < 8; ++y)
	{
		setRow(n, y, 0xff);
	}
}
