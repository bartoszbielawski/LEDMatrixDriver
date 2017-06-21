#include <LEDMatrixDriver.hpp>

// This draw a moving point on your LED matrix and use the scrolling feature of the hardware SPI driver Library by Bartosz Bielawski.
// Example written 19.06.2017 by Marko Oette, www.oette.info 

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


const int ANIM_DELAY = 200;
int x = 0, s = 1, dir = 0, c = 0;
void loop() 
{
  switch( dir )
  {
    case 0:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollDown);
      lmd.setPixel(x,0,true);
      break;
    case 1:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollUp);
      lmd.setPixel(x,7,true);
      break;
    case 2:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollLeft);
      lmd.setPixel(7,x,true);
      break;
    case 3:
      lmd.scroll(LEDMatrixDriver::scrollDirection::scrollRight);
      lmd.setPixel(0,x,true);
      break;   
  };

  // Show the content of the frame buffer
  lmd.display();

  // Increase / Reset the effect counter
  if( c++ > 50 )  // after 50 loos
  {
    if( ++dir > 3 ) // we switch to the next effect
      dir = 0;

    // And reset counter and pixel values
    c=0;
    x=0;
    s=1;

    // Also clearing the buffer gives a better look to this example.
    lmd.clear();
  }
 
  // Move the pixel up and down
  x += s;
  if( x == 7 )
    s = -1;
  else if( x == 0 )
    s = +1;
     
  delay(ANIM_DELAY);  
  
}
