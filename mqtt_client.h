#ifndef _MQTT_CLIENT_H_
#define _MQTT_CLIENT_H_

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>

#define MQTT_SECURITY_MASK 0x7 // 0x1 0x2 0x4
#define MQTT_SECURITY_PLAIN 0x0
#define MQTT_SECURITY_INSECURE 0x1
#define MQTT_SECURITY_ROOT 0x2

#define MQTT_ROOT_MASK 0xF8 // 0x8 0x10 0x20 0x40 0x80
#define MQTT_ROOT_GOOGLE 0x8
#define MQTT_ROOT_LETS_ENCRYPT 0x10
#define MQTT_ROOT_DIGICERT_GLOBAL_G2 0x18

#define MQTT_DIALECT_MASK 0xF00 // 0x100 0x200 0x400 0x800
#define MQTT_DIALECT_HOME_ASSISTANT 0x100
#define MQTT_DIALECT_AZURE_IOT_HUB 0x200

#define MQTT_TYPE_PLAIN_HA 0x100 // 0x0 0x00 0x100
#define MQTT_TYPE_AZURE_IOT_HUB 0x21A // 0x2 0x18 0x200

class SecureMqttClient {
private:
    int _publish(const char *topic, const char *data, bool retain = false);

    int32_t _caType;
    MQTTClient *_mqttClient;
    WiFiClient *_wifiClient;
    char _host[64], _username[64], _password[160];
    char _topic_status[64], _topic_state[64];
    int32_t _port;
    uint32_t _lastConnectAttempt;
    bool _hasDiscovered;
public:
    SecureMqttClient(int32_t caType, const char *host, int32_t port, const char *username, const char *password,
        const void *arg = NULL);
    int getSecurity() { return _caType & MQTT_SECURITY_MASK; }
    int getRootCA() { return _caType & MQTT_ROOT_MASK; }
    int getDialect() { return _caType & MQTT_DIALECT_MASK; }
    void _begin();
    bool _connect();
    void loop();
    int publishDiscovery(const char *topic, const char *json) { return _publish(topic, json, true); }
    int publishState(const void *state, const char *json);
    int publishStatus(const char *message) { return _publish(_topic_status, message, false); }
    int publishTransition(const char *topic, const char *json) { return _publish(topic, json, false); }
    const char * topicState() { return _topic_state; }
    const char * topicStatus() { return _topic_status; }
};

static const char SensorFormat[] = {"{\"uniq_id\":\"%s_%s\",\"obj_id\":\"%s_%s\",\"name\":\"%s\",\"stat_cla\":\"measurement\",\"dev_cla\":\"%s\",\"stat_t\":\"%s\",\"unit_of_meas\":\"%s\",\"val_tpl\":\"{{value_json.%s}}\",\"device\":%s}"};

void mqttBegin();
void mqttEnd();
bool mqttState(const void *state, const char *json);
//void mqttEvents(ChangeFinder *changeFinders[], const char *iso8601);
//bool mqttDetectedState(const char *ddId, const char *data);
void mqttAdvertiseHAN();
void formatSensor(char *topic, char *data, const char *prefix, const char *name, const char *dev_cla, const char *unit_of_meas, const char *field, const char *format = SensorFormat);

void mqttSetup();
void mqttLoop();

#endif // _MQTT_CLIENT_H_