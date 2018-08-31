#include <LEDMatrixDriver.hpp>

// This sketch how to use 7-segment displays with the LEDMatrixDriver library.
// Example written 2018-08-31 by David Agnér, https://github.com/Aggebitter.

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 5;

const int NO_OF_DRIVERS = 3; // Each MAX7219 driver can drive eight 7-segment displays.

               // The LEDMatrixDriver class instance
LEDMatrixDriver lmd(NO_OF_DRIVERS, LEDMATRIX_CS_PIN);

#define X_Last_Digit 23
#define Y_Last_Digit 15
#define Z_Last_Digit 7

void setup() {

  Serial.begin(115200);

  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(4);  // 0 = min, 15 = max
  lmd.setScanLimit(7);  // 0-7: Show 1-8 digits. Beware of currenct restrictions for 1-3 digits! See datasheet.
  lmd.setDecode(0xFF);  // Enable "BCD Type B" decoding for all digits.

}

void loop() {

  LedDisplay(millis() * 100 , millis() * -1000 , millis() * -2);
  delay(10);
}

void LedDisplay(long X_value, long Y_value, long Z_value) {

  // set MAX7219 register as it gets scrambled some times due lack of caps on spi bus
  lmd.setDecode(0xFF);

  // Buffers for int to char conversion, it's needed for blanking zeros and create "-" dash on 7-segment
  char charbufX[9];
  char charbufY[9];
  char charbufZ[9];

  // Filling the char array's with long int value
  // sanity check for input. Must be in range of -9999999 to 9999999 and no buffer overrun I hope
  if (X_value < -9999999 || X_value > 9999999) { 
    for (int i = 0; i < 8; i++) {
      charbufX[i] = '-';
    }
  }
  else dtostrf(X_value, 8, 0, charbufX);

  if (Y_value < -9999999 || Y_value > 9999999) {
    for (int i = 0; i < 8; i++) {
      charbufY[i] = '-';
    }
  }
  else dtostrf(Y_value, 8, 0, charbufY);

  if (Z_value < -9999999 || Z_value > 9999999) {
    for (int i = 0; i < 8; i++) {
      charbufZ[i] = '-';
    }
  }
  else dtostrf(Z_value, 8, 0, charbufZ);

  // Eash char from array is after blanking and signing converted back to uint8_t
  uint8_t digitX;
  uint8_t digitY;
  uint8_t digitZ;

  for (int i = 0; i < 8; i++) {
    // the counter goes thru every char in the respective char array.
    // The Lib-Driver or my 7-segment are reverced in segment order so a negative counter (-i) to set the rigth place for digit

    // X-Axis
    if (charbufX[i] == ' ') lmd.setDigit(X_Last_Digit - i, LEDMatrixDriver::BCD_BLANK);  
    else if (charbufX[i] == '-') lmd.setDigit(X_Last_Digit - i, LEDMatrixDriver::BCD_DASH);
    else {
      digitX = byte(charbufX[i]);
      // fixed place for decimal point
      if (i == 5) lmd.setDigit(X_Last_Digit - i, digitX, X_Last_Digit - i + 3);
      else lmd.setDigit(X_Last_Digit - i, digitX, false);
    }

    // Y-Axis
    if (charbufY[i] == ' ') lmd.setDigit(Y_Last_Digit - i, LEDMatrixDriver::BCD_BLANK);
    else if (charbufY[i] == '-') lmd.setDigit(Y_Last_Digit - i, LEDMatrixDriver::BCD_DASH);
    else {
      digitY = byte(charbufY[i]);
      if (i == 5) lmd.setDigit(Y_Last_Digit - i, digitY, Y_Last_Digit - i + 3);
      else lmd.setDigit(Y_Last_Digit - i, digitY, false);
    }
    // Z-Axis
    if (charbufZ[i] == ' ') lmd.setDigit(Z_Last_Digit - i, LEDMatrixDriver::BCD_BLANK);
    else if (charbufZ[i] == '-') lmd.setDigit(Z_Last_Digit - i, LEDMatrixDriver::BCD_DASH);
    else {
      digitZ = byte(charbufZ[i]);
      if (i == 5) lmd.setDigit(Z_Last_Digit - i, digitZ, Z_Last_Digit - i + 3);
      else lmd.setDigit(Z_Last_Digit - i, digitZ, false);
    }
  }
  lmd.display();
  Serial.println(charbufX);
  Serial.println(charbufY);
  Serial.println(charbufZ);

}
