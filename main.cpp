#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>

#include "boot_button.h"
#include "led_rgb.h"
#include "prefs.h"
#include "wifi_wrapper.h"

void setup() {
    // do not touch strapping pins (2=SCK, 9=RS, 8=/BACKLIGHT) during startup:
    delayMicroseconds(3100);
    ledSetup();

    Serial.begin(115200);
    LOG_INIT(&Serial);
    prefsSetup();
    bootSetup();
    wifiSetup();
    LOGS("=== setup() ===");
}

void loop() {
    bootLoop();
}