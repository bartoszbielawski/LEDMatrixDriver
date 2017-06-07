/*
 * LEDMatrixDriver.cpp
 *
 *  Created on: 30.03.2017
 *      Author: Bartosz Bielawski
 */

#include "LEDMatrixDriver.h"
#include <cassert>
#include <Arduino.h>

LEDMatrixDriver::LEDMatrixDriver(uint8_t N, uint8_t ssPin):
	N(N),
	spiSettings(5000000, MSBFIRST, SPI_MODE0),
	frameBuffer(N*8, 0),
	ssPin(ssPin)
{
	pinMode(ssPin, OUTPUT);
	digitalWrite(ssPin, 1);
	SPI.begin();
	SPI.setHwCs(false);

	setEnabled(false);
	setIntensity(0);
	_sendCommand(LEDMatrixDriver::TEST);			//no test
	_sendCommand(LEDMatrixDriver::DECODE);			//no decode
	_sendCommand(LEDMatrixDriver::SCAN_LIMIT | 7);	//all lines
}

void LEDMatrixDriver::setPixel(uint16_t x, uint16_t y, bool enabled)
{
	assert(y < 8);
	assert(x < (N * 8));

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
	assert(x < (N*8));

	for (uint8_t y = 0; y < 8; y++)
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
	for (uint8_t i = 0; i < N; i++)
	{
		SPI.write16(command);
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
		SPI.write16(cmd);
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
