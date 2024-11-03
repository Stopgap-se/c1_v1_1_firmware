#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <esp32c3/rom/rtc.h>
#include <esp_sntp.h>

#include "device_api.h"
#include "han_reader.h"
#include "led_rgb.h"
#include "mqtt_client.h"
#include "prefs.h"
#include "web_server.h"
#include "wifi_wrapper.h"

#define CRED_SIZE 40

static bool _verifyingCredentials = false;
static AsyncClient *_tcpClient = NULL;
static char _tcpObisHost[128];

void _tcpClientConnect();

void _onTelnetClientDisconnect(void *arg, AsyncClient* telnetClient) {
    LOG("tcp DISCONNECT from %s", arg);
    delay(1000);
    _tcpClientConnect();
}

void _onTelnetClientError(void *arg, AsyncClient* telnetClient, int8_t error) {
    LOG("tcp ERROR %d %s", error, telnetClient->errorToString(error));
}

void _onTelnetClientData(void *arg, AsyncClient* telnetClient, void *data, size_t len) {
    hanOnData((char *)data, len, true);
}

void _tcpClientConnect() {
    int connected = _tcpClient->connect(_tcpObisHost, 23);
    LOG("TCP reconnected to %s: %d", _tcpObisHost, connected);
    _tcpClient->onDisconnect(_onTelnetClientDisconnect, (void *)_tcpObisHost);
    _tcpClient->onError(_onTelnetClientError, (void *)_tcpObisHost);
    _tcpClient->onData(_onTelnetClientData, (void *)_tcpObisHost);
}

void _syncNTP() {
    static const char* ntp_primary = "time.google.com";
    static const char* ntp_secondary = "pool.ntp.org";

    configTime(getDeviceGmtOffset(), getDeviceDaylightOffset(), ntp_primary, ntp_secondary);
}

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
static DNSServer dnsServer;
void _openAP() {
    IPAddress apIP(192, 168, 0, 1);
    bool success;

    WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    success = WiFi.softAP(CurrentlyOne);
    WiFi.setTxPower(WIFI_POWER_5dBm); // lowest TX power when being AP
    dnsServer.start(DNS_PORT, "*", apIP);
    webServerBegin();
    LOG("Open AP %s: %d", CurrentlyOne, success);
    ledBlue();
}

void _closeAP() {
    WiFi.softAPdisconnect();
    dnsServer.stop();
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
      _syncNTP();
      if (_verifyingCredentials) {
          _verifyingCredentials = false;
          prefs()->putString(PREFS_WIFI_SSID, WiFi.SSID());
          prefs()->putString(PREFS_WIFI_PASS, WiFi.psk());
//          prefs()->putInt(PREFS_TIME_GMT_OFFSET, gmtOffset);
          _closeAP();
      }
      else {
        webServerBegin();
        if (_tcpClient && !_tcpClient->connected()) {
            _tcpClientConnect();
        }
        mqttConnect();
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
//    sntp_set_time_sync_notification_cb(_timeSyncNotification);
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
        _openAP();
    }
    WiFi.setHostname(mDnsHostname);
    MDNS.begin(mDnsHostname);
    MDNS.addService("http", "tcp", 80);

    if (prefs()->isKey(PREFS_TELNET_HOST)) {
        prefs()->getString(PREFS_TELNET_HOST, _tcpObisHost, 128);
        _tcpClient = new AsyncClient();
    }
}
