#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <log4arduino.h>

#include "pins.h"

static Adafruit_NeoPixel *pixels = NULL;
static bool ledEnabled = false;
static uint32_t colorBlack, colorRed, colorGreen, colorBlue, colorYellow;

void ledTeardown() {
    ledEnabled = false;
}

void ledShow(uint32_t color) {
    if (ledEnabled) {
        pixels->setPixelColor(0, color);
        pixels->show();
//        LOG("LED %08X", color);
    }
}

void ledRed() {
    ledShow(colorRed);
}

void ledGreen() {
    ledShow(colorGreen);
}

void ledBlue() {
    ledShow(colorBlue);
}

void ledYellow() {
    ledShow(colorYellow);
}

void ledBlack() {
    ledShow(colorBlack);
}

void ledSetup() {
    if (!ledEnabled) {
        pinMode(GPIO_LED_RGB, OUTPUT);
        digitalWrite(GPIO_LED_RGB, LOW);
        delayMicroseconds(300);
        pixels = new Adafruit_NeoPixel(1, GPIO_LED_RGB, NEO_GRB + NEO_KHZ800);
        pixels->begin();
        pixels->setBrightness(64);
        colorBlack = pixels->Color(0, 0, 0);
        colorRed = pixels->Color(70, 0, 0);
        colorGreen = pixels->Color(0, 70, 0);
        colorBlue = pixels->Color(0, 0, 70);
        colorYellow = pixels->Color(70, 70, 0);

        pixels->setPixelColor(0, colorBlack);
        pixels->show();
        ledEnabled = true;
    }
    ledYellow();
    delay(100);
    ledBlack();
}