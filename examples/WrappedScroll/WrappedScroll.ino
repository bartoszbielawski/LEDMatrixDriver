/*
 *  WrappedScroll.ino
 *
 *  Created on: 07.07.2019
 *      Author: Bartosz Bielawski
 * 
 *  This example shows how to use display scrolling with wrapping.
 *  The pattern will be scrolled through the full display in the four directions in an infinite loop.
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

  //draw the pattern
  for (int  i = 0; i < 8 * lmd.getSegments(); i++)
  {
    lmd.setPixel(i, i & 7, true);
    lmd.setPixel(i, 1, true);
    lmd.setPixel(i, 6, true);
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
    int n = 8;
    if ((s ==  LEDMatrixDriver::scrollDirection::scrollLeft) or (s == LEDMatrixDriver::scrollDirection::scrollRight))
      n *= lmd.getSegments();

    for (int i = 0; i < n; i++)
    {
      lmd.scroll(s, true);
      lmd.display();
      delay(100);
    }
  }
}

