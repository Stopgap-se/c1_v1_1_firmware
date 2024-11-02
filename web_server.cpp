#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <ESPAsyncWebServer.h>
#include <esp32c3/rom/rtc.h>

#include "crutch.h"
#include "device_api.h"
#include "prefs.h"
#include "web_server.h"
#include "wifi_wrapper.h"

#include "resources/currently_logo.png.h"
#include "resources/favicon_72x72.png.h"
#include "resources/index.html.h"
#include "resources/devices.html.h"
#include "resources/jquery-3.6.3.min.js.h"
#include "resources/currently.css.h"
#include "resources/currently.js.h"

AsyncWebServer asyncServer(80);
AsyncEventSource _eventSource("/events");

void webServerBegin() {
    asyncServer.begin();
}

void webServerEnd() {
    asyncServer.end();
}

void eventSourceSend(const char *message, const char *event, uint32_t id, uint32_t reconnect) {
    _eventSource.send(message, event, id, reconnect);
}

void eventSourceTeardown() {
    _eventSource.close();
}

size_t _sprintConfig(char *buf) {
    char mqtt[256], name[64];
    memset(mqtt, 0, 256);
    memset(name, 0, 64);
    prefs()->getString(PREFS_LOCAL_URL, mqtt, 256);
    prefs()->getString(PREFS_LOCAL_NAME, name, 64);
    const int imps = prefs()->getInt(PREFS_HW_IMPS, 1000);
    bool crcDisabled = prefs()->getBool(PREFS_CRC_DIS);
    bool autoFotaDisabled = prefs()->getBool(PREFS_AUTO_FOTA_DIS);
    bool pullupDisabled = prefs()->getBool(PREFS_PULLUP_DIS);
//    const int txPwr = wifiGetTxPower();

    return sprintf(buf, "{\"id\":\"%s\",\"v\":\"%s\",\"ssid\":\"%s\",\"localIP\":\"%s\",\"name\":\"%s\",\"rr\":\"%s\",\"rssi\":%d}",
        getDeviceId(), VERSION,
        WiFi.SSID().length() ? WiFi.SSID().c_str() : CurrentlyOne,
        WiFi.localIP().toString().c_str(),
        name,
        getResetReason(rtc_get_reset_reason(0)),
        WiFi.RSSI()); //, txPwr);
}

typedef enum {
    UPT_BOOL,
    UPT_I8,
    UPT_I16,
    UPT_I32,
    UPT_U8,
    UPT_U16,
    UPT_U32,
    UPT_STRING,
    UPT_BLOB,
    UPT_SELECT_I32,
    UPT_SELECT_U8,
    UPT_PASSWORD,
} UserPreferenceType;
static const char _uptNumber[] = {"number"};
static const char _uptSelect[] = {"select"};
static const char *_uptStr[] = {
    "checkbox", _uptNumber, _uptNumber, _uptNumber, _uptNumber, _uptNumber, _uptNumber, "text", "blob", _uptSelect, _uptSelect, "password"
};

struct UserPreference {
    const char *key;
    const char *displayName;
    const UserPreferenceType prefType;
    const int32_t numDefault;
    const char *strDefault;
    const char *placeholder;
    const char *pattern;
    const int16_t maxlength;
};

void _printPref(AsyncResponseStream *response, const UserPreference *up) {
    response->printf("{\"key\":\"%s\",\"name\":\"%s\",\"type\":\"%s\",\"upt\":%d,\"value\":",
        up->key, up->displayName, _uptStr[up->prefType], up->prefType);
    switch (up->prefType) {
    case UPT_BOOL:
        response->printf("%s,\"def\":%s", prefs()->getBool(up->key, up->numDefault) ? "true" : "false", up->numDefault ? "true" : "false");
        break;
    case UPT_SELECT_I32:
    case UPT_I32:
        response->printf("%d,\"def\":%d", prefs()->getInt(up->key, up->numDefault), up->numDefault);
        break;
    case UPT_STRING:
        response->printf("\"%s\",\"def\":\"%s\"", prefs()->getString(up->key, up->strDefault ? up->strDefault : "").c_str(), up->strDefault ? up->strDefault : "");
        break;
    case UPT_SELECT_U8:
    case UPT_U8:
        response->printf("%d,\"def\":%d", prefs()->getUChar(up->key, up->numDefault), up->numDefault);
        break;
    case UPT_PASSWORD:
        response->printf("\"%s\",\"def\":\"%s\"", prefs()->getString(up->key, "").length() ? "***" : "", up->strDefault ? up->strDefault : "");
        break;
    default:
        response->print("NULL");
        break;
    }
    if (up->placeholder) {
        response->printf(",\"placeholder\":\"%s\"", up->placeholder);
    }
    if (up->pattern) {
        response->printf(",\"pattern\":\"%s\"", up->pattern);
    }
    if (up->maxlength) {
        response->printf(",\"maxlength\":%d", up->maxlength);
    }
    response->print("}");
}

#define UP_LIST_SIZE 15
static const UserPreference _prefsList[] = {
    { .key=PREFS_LOCAL_NAME, .displayName="Device name", .prefType=UPT_STRING, .numDefault=0, .strDefault=NULL,
        .placeholder="", .pattern="[^\\\\/]*", .maxlength=39},
    { .key=PREFS_MQTT_HOST, .displayName="MQTT host", .prefType=UPT_STRING, .numDefault=0, .strDefault=NULL,
        .placeholder="hostname or IP", .maxlength=64},
    { .key=PREFS_MQTT_PORT, .displayName="MQTT port", .prefType=UPT_I32, .numDefault=1883, .strDefault=NULL,
        .placeholder="1883"},
    { .key=PREFS_MQTT_USER, .displayName="MQTT user", .prefType=UPT_STRING, .numDefault=0, .strDefault=NULL,
        .placeholder="username", .maxlength=64},
    { .key=PREFS_MQTT_PASS, .displayName="MQTT password", .prefType=UPT_PASSWORD, .numDefault=0, .strDefault=NULL,
        .placeholder="password", .maxlength=160},
    { .key=PREFS_HW_IMPS, .displayName="imps/kWh (1000/tc)", .prefType=UPT_I32, .numDefault=1000, .strDefault=NULL,
        .maxlength=6},
    { .key=PREFS_HW_RTS_MS, .displayName="RTS bugfix (ms)", .prefType=UPT_I32, .numDefault=0, .strDefault=NULL,
        .maxlength=5},
    { .key=PREFS_CRC_DIS, .displayName="CRC disabled", .prefType=UPT_BOOL, .numDefault=false},
    { .key=PREFS_AUTO_FOTA_DIS, .displayName="Auto FOTA disabled", .prefType=UPT_BOOL, .numDefault=false},
    { .key=PREFS_PULLUP_DIS, .displayName="Disable internal pullup", .prefType=UPT_BOOL, .numDefault=false},
    { .key=PREFS_DEVICE_MODE, .displayName="BLINK / HAN", .prefType=UPT_SELECT_U8, .strDefault="15"},
    { .key=PREFS_WIFI_AUTO_TX_POWER, .displayName="Auto txPower", .prefType=UPT_BOOL, .numDefault=false},
    { .key=PREFS_WIFI_TX_POWER, .displayName="WiFi txPower", .prefType=UPT_SELECT_I32, .strDefault="76"},
    { .key=PREFS_CLOUD_MODE, .displayName="Cloud Mode", .prefType=UPT_SELECT_U8, .numDefault=0}, // CLOUD_MODE_FULL},
    { .key=PREFS_TELNET_HOST, .displayName="telnet host", .prefType=UPT_STRING, .numDefault=0, .strDefault=NULL,
        .placeholder="IP address", .pattern="[0-9\\\\.]*", .maxlength=15},
};

void _onGetPrefs(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream(ApplicationJson, 2048);
    response->print("{\"prefs\":[");
    for (int i = 0; i < UP_LIST_SIZE; i++) {
        if (i) {
            response->print(",");
        }
        _printPref(response, &_prefsList[i]);
    }
    response->print("]}");
    request->send(response);
}

void _onGetPref(AsyncWebServerRequest *request) {
    bool found = false;
    int status = 400;
    if (request->hasParam("key", false)) {
        const char *key = request->getParam("key", false)->value().c_str();
        status = 404;
        LOG("onGetPref('%s')", key);
        for (int i = 0; i < UP_LIST_SIZE; i++) {
            found = !strcmp(key, _prefsList[i].key);
            if (found) {
                AsyncResponseStream *response = request->beginResponseStream(ApplicationJson, 128);
                _printPref(response, &_prefsList[i]);
                request->send(response);
                return;
            }
        }
    }
    request->send(status);
}
void _onPostPrefs(AsyncWebServerRequest *request) {
    bool mustRestart = false, found = false;
    int status = 400;
    if (request->hasParam("key", true) && request->hasParam("value", true)) {
        const char *key = request->getParam("key", true)->value().c_str();
        status = 404;
        LOG("onPutPrefs('%s')", key);
        for (int i = 0; i < UP_LIST_SIZE; i++) {
            found = !strcmp(key, _prefsList[i].key);
            if (found) {
                AsyncWebParameter *p = request->getParam("value", true);
                const char *s;
                int v;
                status = 200;
                switch (_prefsList[i].prefType) {
                case UPT_BOOL:
                    s = p->value().c_str();
                    LOG("   %s: %s", key, s);
                    prefs()->putBool(key, !strcmp("true", s));
                    mustRestart = true;
                    break;
                case UPT_I32:
                case UPT_SELECT_I32:
                    v = p->value().toInt();
                    LOG("   %s: %d", key, v);
                    prefs()->putInt(key, v);
                    mustRestart = true;
                    break;
                case UPT_STRING:
                case UPT_PASSWORD:
                    s = p->value().c_str();
                    LOG("   %s: %s", key, s);
                    prefs()->putString(key, s);
                    mustRestart = true;
                    break;
                case UPT_U8:
                case UPT_SELECT_U8:
                    v = p->value().toInt();
                    LOG("   %s: %d", key, v);
                    prefs()->putUChar(key, v);
                    mustRestart = true;
                    break;
                default:
                    status = 400;
                    break;
                }
                if (mustRestart) {
                    request->send(status);
                    crutchRestart(key);
                    return;
                }
                break;
            }
        }
    }
    request->send(status);
}

void _onDeletePrefs(AsyncWebServerRequest *request) {
    bool found = false;
    int status = 400;
    if (request->hasParam("key", true)) {
        const char *key = request->getParam("key", true)->value().c_str();
        status = 404;
        LOG("onDeletePrefs('%s')", key);
        for (int i = 0; i < UP_LIST_SIZE; i++) {
            found = !strcmp(key, _prefsList[i].key);
            if (found) {
                prefs()->remove(key);
                request->send(200);
                crutchRestart(key);
                return;
            }
        }
    }
    request->send(status);
}

void _onEventSourceConnect(AsyncEventSourceClient *client) {
    char configMessage[512];

    client->send(StatusOnline, TopicStatus);
    _sprintConfig(configMessage);
    client->send(configMessage, TopicConfig);
    LOG("esClient connected from %s", client->client()->remoteIP().toString());
}

void _onProvision(AsyncWebServerRequest *request) {
    int gmtOffset;
    AsyncWebParameter* p;
    char uri[40], _ssid[40], _pass[40];
    wl_status_t status = WL_DISCONNECTED;

    memset(_ssid, 0, 40);
    memset(_pass, 0, 40);
    LOGS("doProvision()");
    if (request->hasParam("ssid", true)) {
        p = request->getParam("ssid", true);
        strcpy(_ssid, p->value().c_str());
        LOG("  ssid=%s", _ssid);
        if (request->hasParam("pass", true)) {
            p = request->getParam("pass", true);
            strcpy(_pass, p->value().c_str());
            LOG("  pass.length=%d", strlen(_pass));
        }
        if (request->hasParam("gmtOffset", true)) {
            gmtOffset = request->getParam("gmtOffset", true)->value().toInt();
            LOG("  gmtOffset=%d", gmtOffset);
        }
        else {
            gmtOffset = 3600;
        }
        if (request->hasParam("mode", true)) {
            int v = request->getParam("mode", true)->value().toInt();
            LOG("   mode=0x%02X", v);
            prefs()->putUChar(PREFS_DEVICE_MODE, v);
        }

        memset(uri, 0, 40);
        sprintf(uri, "//CurrentlyOne_%s.local/", getDeviceId());
        LOG("http:%s", uri);
        request->redirect(uri);

        wifiOnProvision(_ssid, _pass);
    }
    else {
        request->send(400);
    }
}

void webServerSetup() {
    asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", resources_index_html_gz, resources_index_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    asyncServer.on("/devices.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", resources_devices_html_gz, resources_devices_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    asyncServer.on("/jquery-3.6.3.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", resources_jquery_3_6_3_min_js_gz, resources_jquery_3_6_3_min_js_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    asyncServer.on("/currently.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", resources_currently_css_gz, resources_currently_css_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    asyncServer.on("/currently.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", resources_currently_js_gz, resources_currently_js_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });
    asyncServer.on("/currently_logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "image/png", resources_currently_logo_png, resources_currently_logo_png_len);
        request->send(response);
    });
    asyncServer.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "image/png", resources_favicon_72x72_png, resources_favicon_72x72_png_len);
        request->send(response);
    });

    asyncServer.on("/api/v1/pref", HTTP_GET, _onGetPref);
    asyncServer.on("/api/v1/prefs", HTTP_GET, _onGetPrefs);
    asyncServer.on("/api/v1/prefs", HTTP_POST, _onPostPrefs);
    asyncServer.on("/api/v1/prefs", HTTP_DELETE, _onDeletePrefs);
    asyncServer.on("/api/v1/provision", HTTP_POST, _onProvision);

    asyncServer.addHandler(&_eventSource);
    _eventSource.onConnect(_onEventSourceConnect);

    LOGS("webServerSetup()");
}