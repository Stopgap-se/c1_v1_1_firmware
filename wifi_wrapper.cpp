#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <WiFi.h>

#include "led_rgb.h"
#include "prefs.h"

void _onWiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_WIFI_READY:
      LOGS("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      LOGS("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      LOGS("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      LOGS("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      ledBlue();
      LOGS("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      LOGS("Disconnected from WiFi, reconnecting...");
//      state = WRAPPER_STATE_SETUP;
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      LOGS("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      LOGS("Obtained IP address:");
      LOGS(WiFi.localIP());
      ledBlack();
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      LOGS("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_AP_START:
      LOGS("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      LOGS("WiFi access point stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      LOGS("WiFi Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      LOGS("WiFi Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      LOGS("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      LOGS("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      LOGS("IPv6 is preferred");
      break;
    default: break;
}}

static String _ssid, _passkey;

void wifiSetup() {
    WiFi.onEvent(_onWiFiEvent);
    WiFi.mode(WIFI_AP_STA); // to allow verification when provisioning
    WiFi.setSleep(true);

    _ssid = prefs()->getString(PREFS_WIFI_SSID, "");
    _passkey = prefs()->getString(PREFS_WIFI_PASS, "");
    if (_ssid.length()) {
        if (_passkey.length()) {
            LOG("%s: ***", _ssid.c_str());
            WiFi.begin(_ssid.c_str(), _passkey.c_str());
        }
        else {
            LOG("%s (Open)", _ssid.c_str());
            WiFi.begin(_ssid.c_str());
        }
    }
    else {
        IPAddress apIP(192, 168, 0, 1);

        WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP("CurrentlyOne");
        WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
    }
//    WiFi.setHostname(mDnsHostname);

    LOGS("wifiSetup()");
}