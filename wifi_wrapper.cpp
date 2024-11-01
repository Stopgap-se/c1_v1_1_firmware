#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <DNSServer.h>
#include <ESPmDNS.h>

#include "device_api.h"
#include "led_rgb.h"
#include "prefs.h"
#include "web_server.h"
#include "wifi_wrapper.h"

#define CRED_SIZE 40

static bool _verifyingCredentials = false;

wl_status_t _connectSTA(const char *ssid, const char *passkey) {
    if (strlen(passkey)) {
        LOG("%s: ***", ssid);
        return WiFi.begin(ssid, passkey);
    }
    else {
        LOG("%s (Open)", ssid);
        return WiFi.begin(ssid);
    }
}

wl_status_t wifiOnProvision(const char *ssid, const char *pass) {

    // verify credentials
    _verifyingCredentials = true;
    return _connectSTA(ssid, pass);
}

#define DNS_PORT 53
void _openAP(const char *mDnsHostname) {
    static DNSServer dnsServer;
    IPAddress apIP(192, 168, 0, 1);
    bool success;

    WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    success = WiFi.softAP(CurrentlyOne);
    WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
    WiFi.setHostname(mDnsHostname);
    MDNS.begin(mDnsHostname);
    dnsServer.start(DNS_PORT, "*", apIP);
    webServerBegin();
    MDNS.addService("http", "tcp", 80);
    LOG("Open AP %s: %d", CurrentlyOne, success);
    ledBlue();
}

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
      if (_verifyingCredentials) {
        ledGreen();
      }
      else {
        ledBlue();
      }
      LOGS("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      LOGS("Disconnected from WiFi, reconnecting...");
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      LOGS("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      LOGS("Obtained IP address:");
      LOGS(WiFi.localIP());
      if (_verifyingCredentials) {
          _verifyingCredentials = false;
          prefs()->putString(PREFS_WIFI_SSID, WiFi.SSID());
          prefs()->putString(PREFS_WIFI_PASS, WiFi.psk());
//          prefs()->putInt(PREFS_TIME_GMT_OFFSET, gmtOffset);
          WiFi.softAPdisconnect();
      }
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

void wifiSetup() {
    char ssid[CRED_SIZE], passkey[CRED_SIZE];
    char mDnsHostname[32];

    WiFi.onEvent(_onWiFiEvent);
    WiFi.mode(WIFI_AP_STA); // to allow verification when provisioning
    WiFi.setSleep(true);
    memset(ssid, 0, CRED_SIZE);
    memset(passkey, 0, CRED_SIZE);

    sprintf(mDnsHostname, "%s_%s", CurrentlyOne, getDeviceId());
    LOG("wifiSetup(%s)", mDnsHostname);
    prefs()->getString(PREFS_WIFI_SSID, ssid, CRED_SIZE);
    prefs()->getString(PREFS_WIFI_PASS, passkey, CRED_SIZE);
    if (strlen(ssid)) {
        _connectSTA(ssid, passkey);
    }
    else {
        _openAP(mDnsHostname);
    }
//    WiFi.setHostname(mDnsHostname);

}