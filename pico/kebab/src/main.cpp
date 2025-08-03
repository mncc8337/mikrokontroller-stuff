#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#define LED 25

#define UP 6
#define DOWN 8
#define LEFT 9
#define RIGHT 7

#define POLLING_INTERVAL 2

uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

Adafruit_USBD_HID usb_hid;

unsigned pincount = 4;
uint8_t pins[] = {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
uint8_t hidcode[] = {
    HID_KEY_ARROW_UP,
    HID_KEY_ARROW_DOWN,
    HID_KEY_ARROW_LEFT,
    HID_KEY_ARROW_RIGHT
};

bool prev_key_pressed = false;

void process_hid() {
    if(!usb_hid.ready()) {
        return;
    }

    uint8_t count = 0;
    uint8_t keycode[6] = {0};

    for(int i = 0; i < pincount; i++) {
        if(digitalRead(pins[i])) {
            continue;
        }

        keycode[count++] = hidcode[i];
        if(count == 6) {
            break;
        }
    }

    if(TinyUSBDevice.suspended() && count) {
        TinyUSBDevice.remoteWakeup();
    }

    if(count) {
        prev_key_pressed = true;
        usb_hid.keyboardReport(0, 0, keycode);
        digitalWrite(LED, HIGH);
    } else if(prev_key_pressed) {
        prev_key_pressed = false;
        usb_hid.keyboardRelease(0);
        digitalWrite(LED, LOW);
    }
}

void setup() {
    // upload code without pressing BOOTSEL
    Serial.begin(115200);

    if(!TinyUSBDevice.isInitialized()) {
        TinyUSBDevice.begin(0);
    }

    usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
    usb_hid.setPollInterval(POLLING_INTERVAL);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    // idk, but adafruit's example do it
    if(TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }

    for(int i = 0; i < pincount; i++) {
        pinMode(pins[i], INPUT_PULLUP);
    }

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
    TinyUSBDevice.task();
#endif

    if(!TinyUSBDevice.mounted()) {
        return;
    }

    process_hid();
    delay(POLLING_INTERVAL);
}
