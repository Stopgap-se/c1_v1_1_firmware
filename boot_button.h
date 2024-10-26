#ifndef _BOOT_BUTTON_H_
#define _BOOT_BUTTON_H_

#include <log4arduino.h>

#include "pins.h"
#include "prefs.h"

static uint32_t bootPinMask;
static volatile uint32_t *bootPinPort, bootButtonDownMillis = 0, bootButtonUpMillis = 0;
static void (*bootBlinkCallback)(uint32_t currentMillis, uint32_t lastMillis);

static void IRAM_ATTR bootButtonHandler(void) {
    uint32_t ms = millis();
    uint8_t pin = (uint8_t)((*bootPinPort & bootPinMask) != 0);

    if (pin) {
        bootButtonUpMillis = ms;
    }
    else {
        bootButtonDownMillis = ms;
    }
}

void bootLoop(void) {
    static uint32_t lastBootBlinkMillis = 0;
    if (bootButtonDownMillis < bootButtonUpMillis) {

        uint32_t dMS = bootButtonUpMillis - bootButtonDownMillis;
        if (10000 <= dMS && dMS < 15000) {
            // factory reset
            prefsFactoryReset();
        }
        bootButtonDownMillis = bootButtonUpMillis;
    }
}

void bootSetup() {
    pinMode(GPIO_BOOT, INPUT_PULLUP);
    bootPinMask = digitalPinToBitMask(GPIO_BOOT);
    bootPinPort = portInputRegister(digitalPinToPort(GPIO_BOOT));
    attachInterrupt(digitalPinToInterrupt(GPIO_BOOT), bootButtonHandler, CHANGE);
    LOG("bootSetup(GPIO%d)", GPIO_BOOT);
}

void bootTeardown() {
    detachInterrupt(digitalPinToInterrupt(GPIO_BOOT));
}

#endif // _BOOT_BUTTON_H_