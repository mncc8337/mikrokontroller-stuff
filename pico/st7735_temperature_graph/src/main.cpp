#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include "linked_list.h"

#define LED 25

#define SPI_SDA 11
#define SPI_SDO 4
#define SPI_SCL 10

#define TFT_CS 9
#define TFT_BLK 13
#define TFT_DC 8
#define TFT_RST 12

#define BMP_CS 5

const int POLLING_INTERVAL = 500;
const int TEMP_HBOUND = 50;
const int TEMP_LBOUND = 20;

linked_list_node* prev_node = 0;
linked_list_node* curr_node;
linked_list_node* head;
linked_list_node* tail;
unsigned data_size = 1;

float temperature = 0;

unsigned* drawn_points;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, SPI_SDA, SPI_SCL, TFT_RST);
Adafruit_BMP280 bmp(BMP_CS, SPI_SDA, SPI_SDO,  SPI_SCL);

void setup(void) {
    curr_node = (linked_list_node*)malloc(sizeof(linked_list_node));
    tail = curr_node;
    pinMode(LED, OUTPUT);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.setCursor(0, 0);

    drawn_points = (unsigned*)malloc(tft.width() * sizeof(unsigned));

    tft.print("screen initialized\n");
    delay(100);

    if(!bmp.begin()) {
        tft.print("failed to initialize BMP280\n");

        while(true) {
            digitalWrite(LED, HIGH);
            delay(500);
            digitalWrite(LED, LOW);
            delay(500);
        }
    }
    bmp.setSampling(
        Adafruit_BMP280::MODE_NORMAL,
        Adafruit_BMP280::SAMPLING_X2,
        Adafruit_BMP280::SAMPLING_X16,
        Adafruit_BMP280::FILTER_X16,
        Adafruit_BMP280::STANDBY_MS_500
    );
    tft.print("BMP280 initialized\n");
    delay(1000);

    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(1);
}

void loop() {
    digitalWrite(LED, HIGH);

    tft.setCursor(tft.width() - 10 - 8 * 6, tft.height() - 8 - 10);
    tft.setTextColor(ST77XX_BLACK);
    tft.print(temperature);
    tft.print(" *C");
    temperature = bmp.readTemperature();
    tft.setCursor(tft.width() - 10 - 8 * 6, tft.height() - 8 - 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(temperature);
    tft.print(" *C");

    if(data_size < tft.width()) {
        curr_node->data = temperature;
        linked_list_node* new_node = (linked_list_node*)malloc(sizeof(linked_list_node));
        new_node->next = 0;
        curr_node->next = new_node;
        prev_node = curr_node;
        head = curr_node;
        curr_node = new_node;
        data_size++;

        // plot the value to screen
        int y = tft.height() - tft.height() / (TEMP_HBOUND - TEMP_LBOUND) * (temperature - TEMP_LBOUND);
        int x = data_size - 2;
        drawn_points[x] = y;

        if(x > 0) {
            tft.drawLine(x - 1, drawn_points[x - 1], x, y, ST77XX_WHITE);
        }
    } else {
        // erase old drawing
        for(int i = 1; i < tft.width(); i++) {
            tft.drawLine(i - 1, drawn_points[i - 1], i, drawn_points[i], ST77XX_BLACK);
        }

        // reuse old node
        curr_node = tail;
        tail = tail->next;
        head->next = curr_node;
        head = curr_node;
        curr_node->data = temperature;
        curr_node->next = 0;

        int y = tft.height() - tft.height() / (TEMP_HBOUND - TEMP_LBOUND) * (temperature - TEMP_LBOUND);
        drawn_points[tft.width() - 1] = y;

        linked_list_node* tmp = tail;
        unsigned i = 0;
        while(tmp->next) {
            int y = tft.height() - tft.height() / (TEMP_HBOUND - TEMP_LBOUND) * (tmp->data - TEMP_LBOUND);
            drawn_points[i] = y;
            if(i > 0) {
                tft.drawLine(i - 1, drawn_points[i - 1], i, drawn_points[i], ST77XX_WHITE);
            }
            i++;
            tmp = tmp->next;
        }
    }

    digitalWrite(LED, LOW);
    delay(POLLING_INTERVAL);
}
