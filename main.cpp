#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>

#include "boot_button.h"
#include "crutch.h"
#include "device_api.h"
#include "han_reader.h"
#include "led_rgb.h"
#include "mqtt_client.h"
#include "obis_parser.h"
#include "prefs.h"
#include "tcp_server.h"
#include "web_server.h"
#include "wifi_wrapper.h"

void setup() {
    // do not touch strapping pins (2=SCK, 9=RS, 8=/BACKLIGHT) during startup:
    delayMicroseconds(3100);
    ledSetup();

    Serial.begin(115200);
    LOG_INIT(&Serial);
    prefsSetup();
    bootSetup(hanInjectMagic);
    deviceSetup();
    obisSetup(crutchOnState);
    hanSetup(crutchOnLine, crutchOnOverflow);
    mqttSetup();
    wifiSetup();
    tcpServerBegin();
    webServerSetup();
    LOGS("=== setup() ===");
}

void loop() {
    bootLoop();
}