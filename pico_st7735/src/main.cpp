#include <Arduino.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define TFT_BLK 13
#define TFT_CS 9
#define TFT_DC 8
#define TFT_RST 12
#define TFT_SDA 11
#define TFT_SCL 10

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RST);
unsigned rotation = 0;

void draw_text(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void setup(void) {
    pinMode(TFT_BLK, OUTPUT);
    tft.initR(INITR_BLACKTAB);
}

void loop() {
    tft.setRotation((rotation++) % 4);
    tft.fillScreen(ST77XX_BLACK);
    draw_text("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);

    for(int i = 0; i < 256; i++) {
        analogWrite(TFT_BLK, i);
        delay(10);
    }

    for(int i = 255; i >= 0; i--) {
        analogWrite(TFT_BLK, i);
        delay(10);
    }

    delay(290);
}
