#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1309AsciiSpi1.h"
// pin definitions

#define CS_PIN  38
#define RST_PIN 10
#define DC_PIN  9

SSD1309AsciiSpi1 oled;
void setoled() {
  oled.begin(&SH1106_128x64, CS_PIN, DC_PIN, RST_PIN);
  oled.setFont(Adafruit5x7);

}
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
	&SPI1, DC_PIN, RST_PIN, CS_PIN);


