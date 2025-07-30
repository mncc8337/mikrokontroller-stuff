#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// VSPI pins
#define SD_CS    5   // Chip Select pin for VSPI
#define SD_SCK   18
#define SD_MISO  19
#define SD_MOSI  23

void setup() {
  Serial.begin(115200);

  // Initialize SPI with VSPI pins
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Initialize SD card
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.println("SD Card initialized.");

  // Example: Write to a file
  File file = SD.open("/test.txt", FILE_WRITE);
  if (file) {
    file.println("Hello from ESP32 VSPI!");
    file.close();
    Serial.println("File written.");
  } else {
    Serial.println("Failed to open file for writing");
  }

  // Example: Read the file
  file = SD.open("/test.txt");
  if (file) {
    Serial.println("Reading file:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }
}

void loop() {
  // Nothing here
}
