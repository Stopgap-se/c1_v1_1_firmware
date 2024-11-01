#ifndef _BOOT_BUTTON_H_
#define _BOOT_BUTTON_H_

#include <log4arduino.h>

#include "pins.h"
#include "prefs.h"

static uint32_t bootPinMask;
static volatile uint32_t *bootPinPort, bootButtonDownMillis = 0, bootButtonUpMillis = 0;
static void (*_magicCallback)(uint32_t dMS);

static void IRAM_ATTR bootButtonHandler(void) {
    uint32_t ms = millis();
    uint8_t pin = (uint8_t)((*bootPinPort & bootPinMask) != 0);

    if (pin) {
        bootButtonUpMillis = ms;
    }
    else {
        bootButtonDownMillis = ms;
    }
    LOG("BOOT button %dms", bootButtonUpMillis-bootButtonDownMillis);
}

void bootLoop(void) {
    static uint32_t lastBootBlinkMillis = 0;

    // FIXME: remove
//    uint32_t ms = millis();
//    if (lastBootBlinkMillis + 500 < ms && _magicCallback) {
//        _magicCallback(ms - lastBootBlinkMillis);
//        lastBootBlinkMillis = ms;
//    }
    // FIXME: end remove

    if (bootButtonDownMillis < bootButtonUpMillis) {

        uint32_t dMS = bootButtonUpMillis - bootButtonDownMillis;
        if (3000 <= dMS && dMS < 5000 && _magicCallback) {
            _magicCallback(dMS);
        }
        else if (10000 <= dMS && dMS < 15000) {
            // factory reset
            prefsFactoryReset();
        }
        bootButtonDownMillis = bootButtonUpMillis;
    }
}

void bootSetup(void (*magicCallback)(uint32_t dMS)) {
    _magicCallback = magicCallback;
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