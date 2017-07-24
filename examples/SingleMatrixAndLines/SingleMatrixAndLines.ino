#include <LEDMatrixDriver.hpp>

// An example for controlling single led matrix and drawing horizontal lines. 4 matrixs are needed for this example.
// Example written on 24.07.2017 by Ng Zouyiu, https://github.com/xJoeWoo.
// Based on the example "SetPixel" written by Marko Oette, www.oette.info.

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 9;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 7;
const int LEDMATRIX_HEIGHT = 7;
const int LEDMATRIX_SEGMENTS = 4;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void setup() {
  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(2);   // 0 = low, 10 = high

  //================ LINES ================
  lmd.setRepeatRow(3, B10101010); // line style repeat on every matrix
  lmd.setRepeatRow16(4, 0x3CE7); // line style repeat on every two matrix

  lmd.display(); // Flush framebuffer

  delay(5000);

  //================ FIRST MATRIX ================

  lmd.setRow(0, 3, B11110000);
  lmd.setRow(0, 4, 0x0f);
  lmd.setColumn(0, 3, B11110000); // note that the cross position is overrided by latest operation
  lmd.setColumn(0, 4, 0x0f);
  lmd.display();

}
int x = 0, y = 0; // start top left, used in second segment
bool s = true;  // start with led on

void loop() {

  //================ SECOND MATRIX ================

  // toggle current pixel in framebuffer
  lmd.setPixel(1, x, y, s);

  // move to next pixel
  if ( x++ >= LEDMATRIX_WIDTH )
  {
    // Return to left
    x = 0;

    // start new line
    if ( y++ >= LEDMATRIX_HEIGHT )
    {
      y = 0;  // need to return to start
      s = !s; // toggle led state
    }
  }

  //================ THIRD MATRIX ================
  if (s)
    lmd.full(2); // turn all led of third matrix on
  else
    lmd.clear(2); // and then off

  //================ FOURTH MATRIX ================
  // it is your playground with some lines on

  lmd.display();

  delay(10);
}
