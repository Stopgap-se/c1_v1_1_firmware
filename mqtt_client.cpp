#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <AsyncMqttClient.h>

#include "device_api.h"
#include "prefs.h"

static AsyncMqttClient *_userClient = NULL;

void _onConnectUser(bool sessionPresent) {
    LOGS("MQTT onConnect");
}

void _onDisconnectUser(AsyncMqttClientDisconnectReason reason) {
    LOG("MQTT onDisconnect, reason=%d", reason);
}

void mqttConnect() {
    LOG("mqttConnect %08x", _userClient);
    if (_userClient) {
        _userClient->connect();
    }
}

void mqttSetup() {
    char host[64], user[64], pass[160];
    int32_t port = prefs()->getInt(PREFS_MQTT_PORT, 1883);
    memset(host, 0, 64);
    memset(user, 0, 64);
    memset(pass, 0, 160);
    prefs()->getString(PREFS_MQTT_HOST, host, 64);
    if (strlen(host)) {
        char clientId[32];

        sprintf(clientId, "one_%d_%s", (uint16_t)(millis()&0xFFFF), getDeviceId());
        prefs()->getString(PREFS_MQTT_USER, user, 64);
        prefs()->getString(PREFS_MQTT_PASS, pass, 160);
        _userClient = new AsyncMqttClient();
        _userClient->onConnect(_onConnectUser);
        _userClient->onDisconnect(_onDisconnectUser);
        _userClient->setServer(IPAddress(10, 0, 1, 168), port);
        _userClient->setClientId(clientId);
        _userClient->setCredentials(user, pass);
        _userClient->setCleanSession(false);
        _userClient->setKeepAlive(60);
        //_userClient->setWill(TopicStatus, )
        LOG("MQTT setup %s %s:%s @ %s:%d", clientId, user, pass, host, port);
    }
}