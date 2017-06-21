/*
 * LEDMatrixDriver.h
 *
 *  Created on: 30.03.2017
 *      Author: Bartosz Bielawski
 */

/*
 * This is a driver for MAX7219 based LED matrix displays.
 * It was written to replace the one already available as a library in Arduino repository.
 *
 * The differences are:
 * 	* This driver uses hardware SPI which makes it much faster (with exception of soft SS)
 * 	* Display or displayRow() has to be used to refresh the display
 * 	* up to 255 segments are supported.
 * 	* can use an external memory or self-allocated buffer
 */

#ifndef LEDMATRIXDRIVER_H_
#define LEDMATRIXDRIVER_H_

#include <SPI.h>

class LEDMatrixDriver
{
	//commands as defined in the datasheet
	const static uint16_t ENABLE =		0x0C00;
	const static uint16_t TEST =	 	0x0F00;
	const static uint16_t INTENSITY =	0x0A00;
	const static uint16_t SCAN_LIMIT =	0x0B00;
	const static uint16_t DECODE =		0x0900;
	
	
	public:
		//with N segments and ssPin as SS,
		//an already allocated buffer can be provided as well
		LEDMatrixDriver(uint8_t N, uint8_t ssPin, uint8_t* frameBuffer = nullptr);
		~LEDMatrixDriver();

		//we don't want to copy the object
		LEDMatrixDriver(const LEDMatrixDriver& other) = delete;
		LEDMatrixDriver(LEDMatrixDriver&& other) = delete;
		LEDMatrixDriver& operator=(const LEDMatrixDriver& other) = delete;

		//all these commands work on ALL segments
		void setEnabled(bool enabled);
		void setIntensity(uint8_t level);
		void setPixel(uint16_t x, uint16_t y, bool enabled);
		void setColumn(uint16_t x, uint8_t value);
		uint8_t getSegments() const {return N;}

		//flush the data to the display
		void display();
		//flush a single row to the display
		void displayRow(uint8_t row) {_displayRow(row);}
		//clear the framebuffer
		void clear() {memset(frameBuffer, 0, 8*N);}
		
		enum class scrollDirection 
		{
			scrollUp = 0,
			scrollDown,
			scrollLeft,
			scrollRight
		};
		
		//scroll the framebuffer 1 pixel in the given direction
		void scroll( scrollDirection direction );

	private:
		void _sendCommand(uint16_t command);
		void _displayRow(uint8_t row);

		const uint8_t N;
		SPISettings spiSettings;
		uint8_t* frameBuffer;
		bool selfAllocated;
		uint8_t ssPin;
};

#endif /* LEDMATRIXDRIVER_H_ */
