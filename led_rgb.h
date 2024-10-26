#ifndef _LED_RGB_H_
#define _LED_RGB_H_

#include <Arduino.h>

void ledSetup();
void ledTeardown();
void ledShow(uint32_t color);
void ledRed();
void ledGreen();
void ledBlue();
void ledYellow();
void ledBlack();

#endif // _LED_RGB_H_