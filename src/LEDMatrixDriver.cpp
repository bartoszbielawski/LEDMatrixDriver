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
