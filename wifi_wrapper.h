#ifndef _WIFI_WRAPPER_H_
#define _WIFI_WRAPPER_H_

#include <WiFi.h>

wl_status_t wifiOnProvision(const char *ssid, const char *pass);

void wifiSetup();
void wifiLoop();

#endif // _WIFI_WRAPPER_H_