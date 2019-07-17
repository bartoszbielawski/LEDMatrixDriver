/*
 *  WrappedScroll.ino
 *
 *  Created on: 15.07.2019
 *      Author: Bartosz Bielawski
 * 
 *  This example shows how to use display scrolling with wrapping.
 * 
 *  WARNING: because the image is cached in the memory this example may not run on platforms
 *           with limited amount of RAM.
 * 
 */

#include <LEDMatrixDriver.hpp>

// Set the Chip Select (CS) pin for the led matrix
const uint8_t LEDMATRIX_CS_PIN = D0;
// Set the number of modules you have daisy-chained together.
const int LEDMATRIX_SEGMENTS = 4;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void setup() 
{
  Serial.begin(115200);

  lmd.setEnabled(true);
  lmd.setIntensity(3);

  //draw chevrons
  const std::array<uint8_t, 8> chevron = {0x81, 0xC3, 0x66, 0x3C, 0x18, 0, 0, 0};

  for (int  i = 0; i < 8 * lmd.getSegments(); i++)
  {
    lmd.setColumn(i, chevron[i & 7]);
  }
}


const static std::array<LEDMatrixDriver::scrollDirection, 4> scrolls = 
{
  LEDMatrixDriver::scrollDirection::scrollDown,
  LEDMatrixDriver::scrollDirection::scrollLeft,
  LEDMatrixDriver::scrollDirection::scrollUp,
  LEDMatrixDriver::scrollDirection::scrollRight,
};

void loop()
{ 
  for (auto s: scrolls)
  {
    int n = 4;
    if ((s ==  LEDMatrixDriver::scrollDirection::scrollLeft) or (s == LEDMatrixDriver::scrollDirection::scrollRight))
      n *= 2 * lmd.getSegments();

    for (int i = 0; i < n; i++)
    {
      lmd.scroll(s, true);
      lmd.display();
      delay(50);
    }
  }
}

