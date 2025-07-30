#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RH_ASK.h>
#include <SPI.h>

#define LED 25

#define TFT_BLK 13
#define TFT_CS 9
#define TFT_DC 8
#define TFT_RST 12
#define TFT_SDA 19
#define TFT_SCL 18

#define RX 22
RH_ASK driver(2000, RX, -1);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int last_packet = -1;
int received_packet = 0;
int lost_packet = 0;

void setup(void) {
    pinMode(LED, OUTPUT);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setTextWrap(true);
    tft.setCursor(0, 0);

    if(!driver.init()) {
        tft.print("rx init failed");
        while(true);
    }

    tft.print("waiting for data ...");
}

void loop() {
    unsigned buf;
    uint8_t buflen = sizeof(buf);
    if(driver.recv((uint8_t*)&buf, &buflen)) {
        digitalWrite(LED, HIGH);
        received_packet++;
        if(last_packet > 0) {
            lost_packet += buf - last_packet - 1;
        }
        last_packet = buf;
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.print("current_packet: ");
        tft.print(buf);
        tft.print("\n");
        tft.print("received packets: ");
        tft.print(received_packet);
        tft.print("\n");
        tft.print("lost packets: ");
        tft.print(lost_packet);
        delay(10);
        digitalWrite(LED, LOW);
    }
}
