#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <ESPAsyncWebServer.h>

#include "device_api.h"
#include "prefs.h"
#include "wifi_wrapper.h"

AsyncWebServer asyncServer(80);

void webServerBegin() {
    asyncServer.begin();
}

void webServerEnd() {
    asyncServer.end();
}

//void _onProvision(AsyncWebServerRequest *request) {
//    if (provisionRequest) {
//        request->send(409);
//    }
//    else {
//        LOGS("onProvision()");
//        provisionRequest = request;
//    }
//}

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
    asyncServer.on("/api/v1/provision", HTTP_POST, _onProvision);
    LOGS("webServerSetup()");
}