#include <LEDMatrixDriver.hpp>

// This sketch will 'flood fill' your LED matrix using the hardware SPI driver Library by Bartosz Bielawski.
// Example written 16.06.2017 by Marko Oette, www.oette.info 

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 9;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 7;  
const int LEDMATRIX_HEIGHT = 7;
const int LEDMATRIX_SEGMENTS = 1;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void setup() {
  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(2);   // 0 = low, 10 = high
}

int x=0, y=0;   // start top left
bool s = true;  // start with led on

void loop() {

  // toggle current pixel in framebuffer
  lmd.setPixel(x, y, s);

  // move to next pixel
  if( x++ >= LEDMATRIX_WIDTH )
  {
    // Return to left
    x=0;
    
    // start new line
    if( y++ >= LEDMATRIX_HEIGHT )
    {
      y = 0;  // need to return to start
      s = !s; // toggle led state
    }
  }
 
  
  // Flush framebuffer
  lmd.display();
  
  delay(10);
}
