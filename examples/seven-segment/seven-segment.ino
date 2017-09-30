#include <LEDMatrixDriver.hpp>

// This sketch how to use 7-segment displays with the LEDMatrixDriver library.
// Example written 2017-09-30 by Søren Thing, https://github.com/Sthing.

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 9;

const int NO_OF_DRIVERS = 1; // Each MAX7219 driver can drive eight 7-segment displays.

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(NO_OF_DRIVERS, LEDMATRIX_CS_PIN);

void setup() {
  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(2);  // 0 = min, 15 = max
  lmd.setScanLimit(7);  // 0-7: Show 1-8 digits. Beware of currenct restrictions for 1-3 digits! See datasheet.
  lmd.setDecode(0xFF);  // Enable "BCD Type B" decoding for all digits.
}

void loop() {
  // Show runtime in seconds for one second
  unsigned long now = millis();
  byte digit;
  do {
    // My display has the digits in reverse order
    // Ie digit 0 is the one on the far right.
    // This matches 10^0 :-)
    unsigned long rest = millis();
    for (digit = 0; digit < 8; digit++) {
      lmd.setDigit(digit, rest % 10, digit == 3);
      rest /= 10;
    }
    lmd.display(); 
    delay(10);
  }
  while (millis() - now < 1000);

  // Show "- HELP -" for one second
  lmd.setDigit(7, LEDMatrixDriver::BCD_DASH);
  lmd.setDigit(6, LEDMatrixDriver::BCD_BLANK);
  lmd.setDigit(5, LEDMatrixDriver::BCD_H);
  lmd.setDigit(4, LEDMatrixDriver::BCD_E);
  lmd.setDigit(3, LEDMatrixDriver::BCD_L);
  lmd.setDigit(2, LEDMatrixDriver::BCD_P);
  lmd.setDigit(1, LEDMatrixDriver::BCD_BLANK);
  lmd.setDigit(0, LEDMatrixDriver::BCD_DASH);
  lmd.display(); 
  delay(1000);
}
