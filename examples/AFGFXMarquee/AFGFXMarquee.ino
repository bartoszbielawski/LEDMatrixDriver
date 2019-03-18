/*
 *  AFGFXMarquee.ino
 *
 *  Created on: 18.03.2019
 *      Author: Bartosz Bielawski
 * 
 *  This example shows how to make a simple marqee using Adafruit_GFX library and LEDMatrixDriver.
 * 
 *  WARNING: because the image is cached in the memory this example may not run on platforms
 *           with limited amount of RAM.
 * 
 *  DEPENDENCIES:
 *  - Adafruit_GFX
 *  - AFBuffer (https://github.com/bartoszbielawski/AFBuffer)
 */

#include <LEDMatrixDriver.hpp>
#include <afbuffer.h>

// Set the Chip Select (CS) pin for the led matrix
const uint8_t LEDMATRIX_CS_PIN = D0;
// Set the number of modules you have daisy-chained together.
const int LEDMATRIX_SEGMENTS = 4;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

//we expect not to need more than 1000 columns for the text
//this buffer is used to render the text
//then columns are copied one after another into the display
const static uint16_t MAX_COLUMNS = 1024;
GFXcanvas1Read canvas(MAX_COLUMNS, 8);

const char message[] = "Hello world! 0123456789 !@#$%^&*()_+ ";
uint16_t messageWidth = 0;

void setup() 
{
  Serial.begin(115200);

  //prepare physical display
  lmd.setEnabled(true);
  lmd.setIntensity(2);
  lmd.clear();
  lmd.display();
  
  //prepare canvas on which the text will be printed
  canvas.setFont();
  canvas.setTextColor(1);
  canvas.setTextSize(0);
  canvas.setTextWrap(true);

  //get how many columns will be needed and print the text
  int16_t x0 = 0, y0 = 0;
  uint16_t w = 0, h = 0;
  canvas.getTextBounds(message, 0,0, &x0, &y0, &w, &h);
  canvas.println(message);

  //save value in a global variable
  Serial.printf("Calculated length in columns: %d", w);
  messageWidth = w;
}

//this counter keeps track of the column that is currently copied from the canvas
int cntr = 0;
//change this value to see a bigger gap after the text ends
static const int afterIterationDelay = 5;

void loop()
{ 
  //move all the pixels to the left
  lmd.scroll(LEDMatrixDriver::scrollDirection::scrollLeft);
  //copy the current column from the canvas into the last column of the  display
  copyBitmap(canvas, cntr, 0, lmd, LEDMATRIX_WIDTH - 1, 0, 1, 8);

  lmd.display();
  cntr++;
  //wrap if needed
  if (cntr >= messageWidth)
    cntr = -afterIterationDelay;

  delay(50);
}

