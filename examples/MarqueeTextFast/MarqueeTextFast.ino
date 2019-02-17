#include <LEDMatrixDriver.hpp>

/*
 *  A very efficient no-whitespace marquee example written 17.02.2019 by James Gohl.
 *    - Using hardware SPI driver by Bartosz Bielawski.
 *      
 *  This example features:
 *  
 *    - Uses a width of 5 pixel in the font-set to save memory.
 *    - Whitespace in characters is ignored giving a cleaner appearance,
 *        character spacing is set by the CHAR_SPACING constant.
 *        Space character width is set by SPACE_WIDTH constant.
 *    - Scrolling is performed at driver level; shifting the buffer contents
 *        left, while writing one column from the current character to the
 *        right-side of the display.
 *    - Timing is performed using elapsed milli seconds.
 *    - If you would like to draw your own fonts using my LEDMatrixPainter, 
 *        you can get it here (https://github.com/jamesgohl/LEDMatrixPainter).
 *    
 *  Wiring for Arduino UNO:
 *    
 *    MAX7219 MODULE(S)        ARDUINO UNO
 *     +-------------+
 *     /             |
 *     \         CLK --------- SCK  (PIN 13)
 *     /          CS --------- CS   (PIN 9)
 *     \         DIN --------- MOSI (PIN 11)
 *     /         GND --------- GND
 *     \         VCC --------- 5V
 *     /             |
 *     +-------------+
 *     
 *  Do NOT use the SS or MISO pin on your Arduino.
 *    
 *  For other models, check here for SPI reference: 
 *  
 *    https://www.arduino.cc/en/Reference/SPI
 *    
 */
// Set the Chip Select (CS) pin for the led matrix
const uint8_t LEDMATRIX_CS_PIN = 9;
// Set the number of modules you have daisy-chained together.
const int LEDMATRIX_SEGMENTS = 8;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

/*
 *  Initialise LEDMatrixDriver
 *    Since there are a lot of varieties of the MAX7219 module out there,
 *    you can flip the column / row addressing per led segment by changing
 *    the flip-segment flags.
 *      0 = Disabled,
 *      1 = Flip columns,
 *      2 = Flip rows,
 *      3 = Flip columns and rows *    
 */
LEDMatrixDriver led(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN, 3);

/*
 *  Scroll Delay is performed using a tick count, 
 *    this ensures scroll rate is closer to the specified delay value
 *    instead of using delay(), which adds to the processing delay.
 *    
 *  Set SCROLL_DELAY to 0 to see the efficiency of this example.
 */
const int SCROLL_DELAY = 25;
// No need to alter this.
uint64_t scrollDelayTimestamp = 0;

/*
 *  Font is derived from "5 by 7 Regular" by Peter Weigel with a few
 *  modifications to clean-up.
 */
const int CHAR_SPACING = 2;
const int SPACE_WIDTH = 2;
byte font[95][5] = {
{B00000000,B00000000,B00000000,B00000000,B00000000}, /* 032 =   */
{B01101111,B01101111,B00000000,B00000000,B00000000}, /* 033 = ! */
{B00000000,B00000011,B00000000,B00000011,B00000000}, /* 034 = " */
{B00010100,B01111111,B00010100,B01111111,B00010100}, /* 035 = # */
{B00100100,B00101010,B01111111,B00101010,B00010010}, /* 036 = $ */
{B00100011,B00010011,B00001000,B01100100,B01100010}, /* 037 = % */
{B00110100,B01001010,B01001010,B00110100,B01010000}, /* 038 = & */
{B00000011,B00000000,B00000000,B00000000,B00000000}, /* 039 = ' */
{B00011100,B00100010,B01000001,B00000000,B00000000}, /* 040 = ( */
{B01000001,B00100010,B00011100,B00000000,B00000000}, /* 041 = ) */
{B00000101,B00000010,B00000101,B00000000,B00000000}, /* 042 = * */
{B00001000,B00001000,B00111110,B00001000,B00001000}, /* 043 = + */
{B11100000,B01100000,B00000000,B00000000,B00000000}, /* 044 = , */
{B00001000,B00001000,B00001000,B00001000,B00001000}, /* 045 = - */
{B01100000,B01100000,B00000000,B00000000,B00000000}, /* 046 = . */
{B01000000,B00110000,B00001000,B00000110,B00000001}, /* 047 = / */
{B00111110,B01010001,B01001001,B01000101,B00111110}, /* 048 = 0 */
{B01000010,B01111111,B01000000,B00000000,B00000000}, /* 049 = 1 */
{B01000010,B01100001,B01010001,B01001001,B01000110}, /* 050 = 2 */
{B00100001,B01000001,B01000101,B01001011,B00110001}, /* 051 = 3 */
{B00011000,B00010100,B00010010,B01111111,B00010000}, /* 052 = 4 */
{B00100111,B01000101,B01000101,B01000101,B00111001}, /* 053 = 5 */
{B00111100,B01001010,B01001001,B01001001,B00110000}, /* 054 = 6 */
{B00000001,B00000001,B01111001,B00000101,B00000011}, /* 055 = 7 */
{B00110110,B01001001,B01001001,B01001001,B00110110}, /* 056 = 8 */
{B00000110,B01001001,B01001001,B00101001,B00011110}, /* 057 = 9 */
{B00110110,B00110110,B00000000,B00000000,B00000000}, /* 058 = : */
{B01110110,B00110110,B00000000,B00000000,B00000000}, /* 059 = ; */
{B00001000,B00010100,B00100010,B01000001,B00000000}, /* 060 = < */
{B00010100,B00010100,B00010100,B00010100,B00010100}, /* 061 = = */
{B01000001,B00100010,B00010100,B00001000,B00000000}, /* 062 = > */
{B00000010,B00000001,B01010001,B00001001,B00000110}, /* 063 = ? */
{B00111110,B01000001,B01011101,B01010101,B01011110}, /* 064 = @ */
{B01111110,B00001001,B00001001,B00001001,B01111110}, /* 065 = A */
{B01111111,B01001001,B01001001,B01001001,B00110110}, /* 066 = B */
{B00111110,B01000001,B01000001,B01000001,B00100010}, /* 067 = C */
{B01111111,B01000001,B01000001,B01000001,B00111110}, /* 068 = D */
{B01111111,B01001001,B01001001,B01001001,B01000001}, /* 069 = E */
{B01111111,B00001001,B00001001,B00001001,B00000001}, /* 070 = F */
{B00111110,B01000001,B01001001,B01001001,B01111010}, /* 071 = G */
{B01111111,B00001000,B00001000,B00001000,B01111111}, /* 072 = H */
{B01000001,B01000001,B01111111,B01000001,B01000001}, /* 073 = I */
{B00100000,B01000001,B01000001,B00111111,B00000001}, /* 074 = J */
{B01111111,B00001000,B00010100,B00100010,B01000001}, /* 075 = K */
{B01111111,B01000000,B01000000,B01000000,B01000000}, /* 076 = L */
{B01111111,B00000010,B00000100,B00000010,B01111111}, /* 077 = M */
{B01111111,B00000110,B00001000,B00110000,B01111111}, /* 078 = N */
{B00111110,B01000001,B01000001,B01000001,B00111110}, /* 079 = O */
{B01111111,B00010001,B00010001,B00010001,B00001110}, /* 080 = P */
{B00111110,B01000001,B01010001,B00100001,B01011110}, /* 081 = Q */
{B01111111,B00001001,B00011001,B00101001,B01000110}, /* 082 = R */
{B00100110,B01001001,B01001001,B01001001,B00110010}, /* 083 = S */
{B00000001,B00000001,B01111111,B00000001,B00000001}, /* 084 = T */
{B00111111,B01000000,B01000000,B01000000,B00111111}, /* 085 = U */
{B00011111,B00100000,B01000000,B00100000,B00011111}, /* 086 = V */
{B00111111,B01000000,B00110000,B01000000,B00111111}, /* 087 = W */
{B01100011,B00010100,B00001000,B00010100,B01100011}, /* 088 = X */
{B00000111,B00001000,B01110000,B00001000,B00000111}, /* 089 = Y */
{B01100001,B01010001,B01001001,B01000101,B01000011}, /* 090 = Z */
{B01111111,B01000001,B01000001,B00000000,B00000000}, /* 091 = [ */
{B00000001,B00000110,B00001000,B00110000,B01000000}, /* 092 = \ */
{B01000001,B01000001,B01111111,B00000000,B00000000}, /* 093 = ] */
{B00000100,B00000010,B00000001,B00000010,B00000100}, /* 094 = ^ */
{B01000000,B01000000,B01000000,B01000000,B01000000}, /* 095 = _ */
{B00000011,B00000000,B00000000,B00000000,B00000000}, /* 096 = ' */
{B00100000,B01010100,B01010100,B01010100,B01111000}, /* 097 = a */
{B01111111,B00101000,B01000100,B01000100,B00111000}, /* 098 = b */
{B00111000,B01000100,B01000100,B01000100,B00101000}, /* 099 = c */
{B00111000,B01000100,B01000100,B00101000,B01111111}, /* 100 = d */
{B00111000,B01010100,B01010100,B01010100,B00011000}, /* 101 = e */
{B00000100,B01111110,B00000101,B00000001,B00000010}, /* 102 = f */
{B00011000,B10100100,B10100100,B10100100,B01111100}, /* 103 = g */
{B01111111,B00000100,B00000100,B00000100,B01111000}, /* 104 = h */
{B01000100,B01111101,B01000000,B00000000,B00000000}, /* 105 = i */
{B01000000,B10000000,B10000100,B01111101,B00000000}, /* 106 = j */
{B01111111,B00010000,B00010000,B00101000,B01000100}, /* 107 = k */
{B01000001,B01111111,B01000000,B00000000,B00000000}, /* 108 = l */
{B01111100,B00000100,B01111100,B00000100,B01111000}, /* 109 = m */
{B01111100,B00001000,B00000100,B00000100,B01111000}, /* 110 = n */
{B00111000,B01000100,B01000100,B01000100,B00111000}, /* 111 = o */
{B11111100,B00100100,B00100100,B00100100,B00011000}, /* 112 = p */
{B00011000,B00100100,B00100100,B00100100,B11111100}, /* 113 = q */
{B01111100,B00001000,B00000100,B00000100,B00001000}, /* 114 = r */
{B01001000,B01010100,B01010100,B01010100,B00100000}, /* 115 = s */
{B00000100,B00111110,B01000100,B01000000,B00100000}, /* 116 = t */
{B00111100,B01000000,B01000000,B00100000,B01111100}, /* 117 = u */
{B00011100,B00100000,B01000000,B00100000,B00011100}, /* 118 = v */
{B00111100,B01000000,B00110000,B01000000,B00111100}, /* 119 = w */
{B01000100,B00101000,B00010000,B00101000,B01000100}, /* 120 = x */
{B00000100,B01001000,B00110000,B00001000,B00000100}, /* 121 = y */
{B01000100,B01100100,B01010100,B01001100,B01000100}, /* 122 = z */
{B00001000,B00110110,B01000001,B00000000,B00000000}, /* 123 = { */
{B01111111,B00000000,B00000000,B00000000,B00000000}, /* 124 = | */
{B01000001,B00110110,B00001000,B00000000,B00000000}, /* 125 = } */
{B00011000,B00000100,B00001000,B00010000,B00001100}  /* 126 = ~ */
};
const int CHAR_WIDTH = sizeof(font[0]);

/*
 *  Text to scroll, this is hard-coded but if you want to have dynamic
 *    text from Serial input, you should Set scrollWhitespace to
 *    LEDMATRIX_WIDTH , wait for this to return to zero so the previous 
 *    text scrolls away, then reset all of the vars in this section to 
 *    their defaults.  You can probably get away with 1000+ characters.
*/
// Text to scroll.
char text[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'abcdefghijklmnopqrstuvwxyz{|}~";
int textLen = 0;
// Max is textLen - 1.
int textIndex = 0;
// Max is CHAR_WIDTH - 1.
int colIndex = 0;
// Used to calculate start and finsh columns per character.
int colLower = -1;
int colUpper = CHAR_WIDTH;
// Used to set whitespace to the given number of columns,
//   if > 0 in every loop this value will decrease by 1 and
//   draw nothing while scrolling.
// At the end of each character we set this to CHAR_SPACING.
// At the end of the text we set this to LEDMATRIX_WIDTH.
int scrollWhitespace = 0;



void setup() {

  led.setEnabled(true);

  // LED brightness (0 - 10).
  led.setIntensity(2);
  textLen = strlen(text);
  
  Serial.begin(9600);
}

void loop()
{
  // millis() will reset back to zero eventually so we need to handle this.
  if (millis() < 1)
    scrollDelayTimestamp = 0;
  if (millis() < scrollDelayTimestamp)
    return;
  scrollDelayTimestamp = millis() + SCROLL_DELAY;

  // Shift everything left 1 led column.
  led.scroll(3);

  if (scrollWhitespace > 0)
    // Deal with whitespace - scroll but don't set LEDs.
    scrollWhitespace--;
  else
    // Write the next column of leds to the right.
    writeNext();
  
  // The driver is buffering so we need write.
  led.display();
}

void writeNext()
{
  byte asc = text[textIndex] - 32;

  // We need to handle space chracter since ignoring whitespace.
  if (asc == 0)
  {
    scrollWhitespace = SPACE_WIDTH;
    // Force change to next character in text[].
    if (textIndex++ == textLen - 1)
    {
      textIndex = 0;
      scrollWhitespace = LEDMATRIX_WIDTH;
    }
    return;
  }

  // Not set for this character, so find start column.
  if (colLower == -1)
  {
    for (int c = 0; c < CHAR_WIDTH; c++)
    {
      if (font[asc][c] != 0)
      {
        colLower = c;
        colIndex = c;
        break;
      }
    }
  }
  
  // Not set for this character, so find finish column.
  if (colUpper == CHAR_WIDTH)
  {
    for (int c = CHAR_WIDTH - 1; c > -1; c--)
    {
      if (font[asc][c] != 0)
      {
        colUpper = c;
        break;
      }
    }
  }

  // Write the column to the right of the display.
  for (int y = 0; y < 8; y++)
  {
    if ((font[asc][colIndex] & 1<<y) != 0)
      led.setPixel(LEDMATRIX_WIDTH - 1, y, true);
  }

  // Check if end of current character.
  if (colIndex++ == colUpper)
  {
    // Reset column start and finsh.
    colLower = -1; colUpper = CHAR_WIDTH;
    // Add whitespace.
    scrollWhitespace = CHAR_SPACING;

    // Check if end of text string.
    if (textIndex++ == textLen - 1)
    {
      textIndex = 0;
      scrollWhitespace = LEDMATRIX_WIDTH;
    }
  }
}
