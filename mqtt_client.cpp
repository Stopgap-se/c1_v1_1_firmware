#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <base64.hpp>
#include <esp32c3/rom/rtc.h>

#include "device_api.h"
#include "mqtt_client.h"
#include "web_server.h"
#include "prefs.h"

static bool _wifiConnected = false;

bool _onConnected(SecureMqttClient *client);

static const char * _returnCodeStr(int returnCode) {
  switch (returnCode) {
    case (LWMQTT_CONNECTION_ACCEPTED):
      return "OK";
    case (LWMQTT_UNACCEPTABLE_PROTOCOL):
      return "LWMQTT_UNACCEPTABLE_PROTOCOL";
    case (LWMQTT_IDENTIFIER_REJECTED):
      return "LWMQTT_IDENTIFIER_REJECTED";
    case (LWMQTT_SERVER_UNAVAILABLE):
      return "LWMQTT_SERVER_UNAVAILABLE";
    case (LWMQTT_BAD_USERNAME_OR_PASSWORD):
      return "LWMQTT_BAD_USERNAME_OR_PASSWORD";
    case (LWMQTT_NOT_AUTHORIZED):
      return "LWMQTT_NOT_AUTHORIZED";
    case (LWMQTT_UNKNOWN_RETURN_CODE):
      return "LWMQTT_UNKNOWN_RETURN_CODE";
    default:
      return "This return code should never be reached.";
  }
}

#define NO_CUSTOM_BROKER -22
static const char * _lastErrorString(int lastError) {
  switch(lastError) {
    case (LWMQTT_SUCCESS):
			return "LWMQTT_SUCCESS";
    case (LWMQTT_BUFFER_TOO_SHORT):
			return "LWMQTT_BUFFER_TOO_SHORT";
    case (LWMQTT_VARNUM_OVERFLOW):
			return "LWMQTT_VARNUM_OVERFLOW";
    case (LWMQTT_NETWORK_FAILED_CONNECT):
			return "LWMQTT_NETWORK_FAILED_CONNECT";
    case (LWMQTT_NETWORK_TIMEOUT):
			return "LWMQTT_NETWORK_TIMEOUT";
    case (LWMQTT_NETWORK_FAILED_READ):
			return "LWMQTT_NETWORK_FAILED_READ";
    case (LWMQTT_NETWORK_FAILED_WRITE):
			return "LWMQTT_NETWORK_FAILED_WRITE";
    case (LWMQTT_REMAINING_LENGTH_OVERFLOW):
			return "LWMQTT_REMAINING_LENGTH_OVERFLOW";
    case (LWMQTT_REMAINING_LENGTH_MISMATCH):
			return "LWMQTT_REMAINING_LENGTH_MISMATCH";
    case (LWMQTT_MISSING_OR_WRONG_PACKET):
			return "LWMQTT_MISSING_OR_WRONG_PACKET";
    case (LWMQTT_CONNECTION_DENIED):
			return "LWMQTT_CONNECTION_DENIED";
    case (LWMQTT_FAILED_SUBSCRIPTION):
			return "LWMQTT_FAILED_SUBSCRIPTION";
    case (LWMQTT_SUBACK_ARRAY_OVERFLOW):
			return "LWMQTT_SUBACK_ARRAY_OVERFLOW";
    case (LWMQTT_PONG_TIMEOUT):
			return "LWMQTT_PONG_TIMEOUT";
	case (NO_CUSTOM_BROKER):
	        return "NOMQTT";
    default:
			return "unknown";
  }
}

void _onMessage(MQTTClient *client, const char topic[], const char payload[], int length) {
    LOGS("MqttClient.onMessage");
    LOGS(topic);
    LOGS(payload);
}

SecureMqttClient::SecureMqttClient(int32_t caType, const char *host, int32_t port,
        const char *username, const char *password,
        const void *arg) {
    _wifiClient = NULL;
    _mqttClient = NULL;

    _caType = caType;
    strcpy(_host, host);
    _port = port;
    strcpy(_username, username);
    strcpy(_password, password);
    _hasDiscovered = false;
    LOG("MQTT.<init>(0x%03X) sec=%X ca=%X dia=%X %s", _caType, _caType & MQTT_SECURITY_MASK,
        _caType & MQTT_ROOT_MASK, _caType & MQTT_DIALECT_MASK, _host);
}

void SecureMqttClient::_begin() {
    if (!_wifiClient) {
        WiFiClientSecure *_secure = NULL;
        switch (_caType & MQTT_SECURITY_MASK) {
        case MQTT_SECURITY_PLAIN:
            _wifiClient = new WiFiClient();
            LOG("  .plain(0x%03X)", _caType & MQTT_SECURITY_MASK);
            break;
        case MQTT_SECURITY_INSECURE:
            _secure = new WiFiClientSecure();
            _secure->setInsecure();
            LOG("  .setInsecure(0x%03X)", _caType & MQTT_SECURITY_MASK);
            break;
        default:
            _secure = new WiFiClientSecure();
            break;
        }
        if (!_wifiClient) {
            _wifiClient = _secure;
        }
    }

    if (!_mqttClient) {
        LOG("MQTT._begin %s:%d", _host, _port);

        memset(_topic_status, 0, 64);
        memset(_topic_state, 0, 64);
        switch (_caType & MQTT_DIALECT_MASK) {
        default:
            _mqttClient = new MQTTClient(1024);
            _mqttClient->begin(*_wifiClient);
            _mqttClient->setHost(_host, _port);
            _mqttClient->onMessageAdvanced(_onMessage);
            sprintf(_topic_status, "currently/one/%s/status", getDeviceId());
            sprintf(_topic_state, "currently/one/%s/state", getDeviceId());
            _mqttClient->setWill(_topic_status, "offline");
            _mqttClient->setKeepAlive(60);
            _mqttClient->setCleanSession(false);
            break;
        }
        _lastConnectAttempt = 0;
    }
}

bool SecureMqttClient::_connect() {
    const uint32_t ms = millis();
    bool connected = _mqttClient ? _mqttClient->connected() : _wifiClient->connected();
    #define CONNECT_RETRY_PERIOD 30000

    if (!connected && (!_lastConnectAttempt || _lastConnectAttempt + CONNECT_RETRY_PERIOD < ms)) {
        char clientId[64], topic_sub[64], token[160], resourceUri[160];
        WiFiClientSecure *_secure = (WiFiClientSecure *)_wifiClient;
        _lastConnectAttempt = ms;

        strcpy(token, "");
        strcpy(topic_sub, "");
//        switch (_caType & MQTT_ROOT_MASK) {
//        case MQTT_ROOT_DIGICERT_GLOBAL_G2:
//            _secure->setCACert(DigiCertRootG2);
//            break;
//        case MQTT_ROOT_LETS_ENCRYPT:
//            _secure->setCACert(LetsEncryptRoot);
//            break;
//        case MQTT_ROOT_GOOGLE:
//            _secure->setCACert(GoogleCertRoot);
//            break;
//        default:
//            break;
//        }
        switch (_caType & MQTT_DIALECT_MASK) {
        default:
            sprintf(clientId, "one_%d_%s", (uint16_t)(ms&0xFFFF), getDeviceId());
            sprintf(topic_sub, "currently/one/%s/cmd/#", getDeviceId());
            connected = _mqttClient->connect(clientId,
                strlen(_username) ? _username : nullptr,
                strlen(_password) ? _password : nullptr);
            if (connected) {
                if (!publishStatus(StatusOnline)) {
                    LOG("  ERROR publish %s %s", _topic_status, StatusOnline);
                }
            }
            break;
        }
        LOG("MQTT._connected: %d, topic_status: %s", connected, _topic_status);

        if (!connected) {
            if (_mqttClient) {
                LOG("MQTT.connect(%s)=%d last=%d", clientId, connected, _mqttClient->lastError());
                LOG("  error=%s", _lastErrorString(_mqttClient->lastError()));
                LOG("  code=%s", _returnCodeStr(_mqttClient->returnCode()));
                LOG("  user=%s", _username);
                LOG("  pass=%s", strlen(token) ? token : _password);
                // Clean up the client
                _mqttClient->disconnect();
            }
        }
        else {
            _lastConnectAttempt = ms - CONNECT_RETRY_PERIOD;

            if (_mqttClient && strlen(topic_sub)) {
                if (_mqttClient->subscribe(topic_sub, 0)) {
                    LOG("MQTT.subscribed to %s", topic_sub);
                }
                else {
                    LOG("ERROR subscribing to %s", topic_sub);
                }
            }
        }
    }
    if (connected && !_hasDiscovered) {
        switch (_caType & MQTT_DIALECT_MASK) {
        case MQTT_DIALECT_AZURE_IOT_HUB:
            break;
        default:
            _onConnected(this);
            break;
        }
        _hasDiscovered = true;
    }
    return connected;
}

void SecureMqttClient::loop() {
    if (_mqttClient) {
        _mqttClient->loop();
    }
    // TODO: poll for C2D messages
}

int SecureMqttClient::publishState(const void *state, const char *json) {
    int res = 0;
    if (_connect()) {

        switch (_caType & MQTT_DIALECT_MASK) {
        case MQTT_DIALECT_AZURE_IOT_HUB:
            LOGS(_topic_state);
            res = _publish(_topic_state, json, false);
            if (res) {
                LOGS(json);
            }
            break;
        default:
            res = _publish(_topic_state, json, false);
            break;
        }
    }
    return res;
}

int SecureMqttClient::_publish(const char *topic, const char *data, bool retain) {
    int res = 0;
    switch (_caType & MQTT_DIALECT_MASK) {
    default:
        res = _mqttClient->publish(topic, data, strlen(data), retain, 0);
        break;
    }
    return res;
}

// Initialize WiFi and MQTT for this board
static Preferences *localPreferences;
#define TOPIC_SIZE 48
static char topic_config[TOPIC_SIZE], topic_status[TOPIC_SIZE], topic_state[TOPIC_SIZE], topic_progress[TOPIC_SIZE];
static const char topic_base[] = {"currently/one"};
static SecureMqttClient *userClient = NULL, *dmClient = NULL;

void mqttBegin() {
    _wifiConnected = true;
    LOG("mqttBegin: %d", _wifiConnected);
    if (userClient) {
        userClient->_begin();
        userClient->_connect();
    }

    if (dmClient) {
        dmClient->_begin();
        dmClient->_connect();
    }
}

void mqttEnd() {
    _wifiConnected = false;
}

bool mqttState(const void *state, const char *json) {
    int res = 0;
    // must not break Root CA Cert for currently.cloud:
    if (userClient && MQTT_SECURITY_PLAIN == userClient->getSecurity()) {
        res = userClient->publishState(state, json);
    }
    if (dmClient && MQTT_SECURITY_PLAIN == dmClient->getSecurity()) {
        res = dmClient->publishState(state, json);
    }
    return res;
}

void _formatDevice(char *data, const char *identifier, const char *name,
        const char *model, const char *manufacturer, const char *swVersion) {
    sprintf(data, "{\"identifiers\":[\"%s\"],\"name\":\"%s\",\"model\":\"%s\",\"manufacturer\":\"%s\",\"sw_version\":\"%s\"}",
        identifier, name, model, manufacturer, swVersion);
}
//void _registerAppliance(SecureMqttClient *client, DeviceProfileProto *dp) {
//    char topic[128], stat_t[128], data[512], device[256];
//    sprintf(data, "%u", dp->updatedTimestamp);
//    _formatDevice(device, dp->idPrefix, dp->name, dp->profileId, dp->categoryId, data);
//    sprintf(stat_t, "%s/%s/detect", topic_base, dp->idPrefix);
//
//    sprintf(topic, "homeassistant/binary_sensor/%s/config", dp->idPrefix);
//    sprintf(data, "{\"uniq_id\":\"%s\",\"obj_id\":\"%s\",\"name\":\"%s\",\"stat_t\":\"%s\",\"val_tpl\":\"{{value_json.bs}}\",\"device\":%s}",
//        dp->idPrefix, dp->idPrefix, dp->name, stat_t, device);
//    client->publishDiscovery(topic, data);
//
//    sprintf(topic, "homeassistant/sensor/total_%s/config", dp->idPrefix);
//    sprintf(data, "{\"uniq_id\":\"total_%s\",\"obj_id\":\"total_%s\",\"name\":\"%s used\",\"dev_cla\":\"energy\",\"stat_t\":\"%s\",\"stat_cla\":\"total_increasing\",\"unit_of_meas\":\"Wh\",\"val_tpl\":\"{{value_json.Wh}}\",\"device\":%s}",
//        dp->idPrefix, dp->idPrefix, dp->name, stat_t, device);
//    client->publishDiscovery(topic, data);
//
//    sprintf(topic, "homeassistant/sensor/power_%s/config", dp->idPrefix);
//    sprintf(data, "{\"uniq_id\":\"power_%s\",\"obj_id\":\"power_%s\",\"name\":\"%s power\",\"dev_cla\":\"power\",\"stat_t\":\"%s\",\"unit_of_meas\":\"W\",\"val_tpl\":\"{{value_json.W}}\",\"device\":%s}",
//        dp->idPrefix, dp->idPrefix, dp->name, stat_t, device);
//    client->publishDiscovery(topic, data);
//}
//
//void _registerAppliances(SecureMqttClient *client) {
//    for (int i = 0; i < ddCount(); i++) {
//        DeviceProfileProto dp= DeviceProfileProto_init_zero;
//        if (ddGetProfile(&dp, i)) {
//            _registerAppliance(client, &dp);
//        }
//    }
//}

bool mqttDetectedState(const char *ddId, const char *data) {
    char stat_t[128];
    sprintf(stat_t, "%s/%s/detect", topic_base, ddId);
    LOG("%s %s", stat_t, data);
    return userClient ? userClient->publishTransition(stat_t, data) : false;
}

void _formatCurrentlyDevice(char *data) {
    char identifier[32], name[32];
    sprintf(identifier, "C1_%s", getDeviceId());
    sprintf(name, "Currently One %s", getDeviceId());
    _formatDevice(data, identifier, name, "Currently One", "Stopgap AB", VERSION);
}

static const char EnergySensorFormat[] = {"{\"uniq_id\":\"%s_%s\",\"obj_id\":\"%s_%s\",\"name\":\"%s\",\"dev_cla\":\"%s\",\"stat_cla\":\"total_increasing\",\"stat_t\":\"%s\",\"unit_of_meas\":\"%s\",\"val_tpl\":\"{{value_json.%s}}\",\"device\":%s}"};
static const char TimeSensorFormat[] = {"{\"uniq_id\":\"%s_%s\",\"obj_id\":\"%s_%s\",\"name\":\"%s\",\"dev_cla\":\"%s\",\"stat_t\":\"%s\",%s\"val_tpl\":\"{{value_json.%s}}\",\"device\":%s}"};
void formatSensor(char *topic, char *data, const char *prefix, const char *name, const char *dev_cla,
        const char *unit_of_meas, const char *field, const char *format) { // default format is SensorFormat
    char device[160];
    _formatCurrentlyDevice(device);
    sprintf(data, format,
      prefix, getDeviceId(),
      prefix, getDeviceId(),
      name, dev_cla, topic_state, unit_of_meas, field,
      device);
    sprintf(topic, "homeassistant/sensor/%s_%s/config", prefix, getDeviceId());
}
void _formatBoot(char *topic, char *data, const char *prefix, const char *name, const char *dev_cla, const char *initial) {
    char device[160];
    _formatCurrentlyDevice(device);
    sprintf(data, "{\"uniq_id\":\"%s_%s\",\"obj_id\":\"%s_%s\",\"name\":\"%s\",\"dev_cla\":\"%s\",\"stat_t\":\"%s\",\"val_tpl\":\"{{value_json.%s}}\",\"init\":\"%s\",\"device\":%s}",
      prefix, getDeviceId(),
      prefix, getDeviceId(),
      name, dev_cla, topic_config, "t", initial,
      device);
    sprintf(topic, "homeassistant/sensor/%s_%s/config", prefix, getDeviceId());
}

bool _onConnected(SecureMqttClient *client) {
    char topic[128], data[512];
    uint8_t countdown = 33;

    sprintf(topic_config, "currently/one/%s/config", getDeviceId());
    strcpy(topic_state, client->topicState());
    strcpy(topic_status, client->topicStatus());
    for (; 0 < countdown; countdown--) {
        memset(data, 0, 512);
        memset(topic, 0, 128);
        switch (countdown) {
        case 33: formatSensor(topic, data, "L1", "Currently L1", "power", "W", "L1"); break;
        case 32: formatSensor(topic, data, "Qh", "Currently Reactive Returned", "energy", "Wh", "Qh", EnergySensorFormat); break;
        case 31: formatSensor(topic, data, "Rh", "Currently Reactive Used", "energy", "Wh", "Rh", EnergySensorFormat); break;
        case 30: formatSensor(topic, data, "Q", "Currently Reactive Returning", "reactive_power", "var", "Q"); break;
        case 29: formatSensor(topic, data, "R", "Currently Reactive", "reactive_power", "var", "R"); break;
        case 28: formatSensor(topic, data, "return", "Currently Returning", "power", "W", "P"); break;
        case 27: formatSensor(topic, data, "returned", "Currently Returned", "energy", "Wh", "Ph", EnergySensorFormat); break;
        case 26: formatSensor(topic, data, "Q3", "Currently Q3", "reactive_power", "var", "Q3"); break;
        case 25: formatSensor(topic, data, "Q2", "Currently Q2", "reactive_power", "var", "Q2"); break;
        case 24: formatSensor(topic, data, "Q1", "Currently Q1", "reactive_power", "var", "Q1"); break;
        case 23: formatSensor(topic, data, "R3", "Currently R3", "reactive_power", "var", "R3"); break;
        case 22: formatSensor(topic, data, "R2", "Currently R2", "reactive_power", "var", "R2"); break;
        case 21: formatSensor(topic, data, "R1", "Currently R1", "reactive_power", "var", "R1"); break;
        case 20: formatSensor(topic, data, "P3", "Currently P3", "power", "W", "P3"); break;
        case 19: formatSensor(topic, data, "P2", "Currently P2", "power", "W", "P2"); break;
        case 18: formatSensor(topic, data, "P1", "Currently P1", "power", "W", "P1"); break;
        case 17: formatSensor(topic, data, "I3", "Currently I3", "current", "A", "I3"); break;
        case 16: formatSensor(topic, data, "I2", "Currently I2", "current", "A", "I2"); break;
        case 15: formatSensor(topic, data, "I1", "Currently I1", "current", "A", "I1"); break;
        case 14: formatSensor(topic, data, "U3", "Currently U3", "voltage", "V", "U3"); break;
        case 13: formatSensor(topic, data, "U2", "Currently U2", "voltage", "V", "U2"); break;
        case 12: formatSensor(topic, data, "U1", "Currently U1", "voltage", "V", "U1"); break;
        case 11: formatSensor(topic, data, "L3", "Currently L3", "power", "W", "L3"); break;
        case 10: formatSensor(topic, data, "L2", "Currently L2", "power", "W", "L2"); break;
//        case 9: if (mode&DEVICE_MODE_BLINK) {
//                formatSensor(topic, data, "blink", "Currently Blink Power", "power", "W", "bW");
//            }
//            break;
//        case 8: if (mode&DEVICE_MODE_BLINK) {
//                formatSensor(topic, data, "bWh", "Currently imps", "energy", "Wh", "bWh", EnergySensorFormat);
//            }
//            break;
        case 7: strcpy(data, StatusOnline); strcpy(topic, topic_status); break;
        case 6: _formatBoot(topic, data, "boot", "Currently Boot", "timestamp", getDeviceDateTimeStr()); break;
        case 5: formatSensor(topic, data, "time", "Currently Seen", "timestamp", "", "t", TimeSensorFormat); break;
        case 4: formatSensor(topic, data, "power", "Currently Using", "power", "W", "W"); break;
        case 3: formatSensor(topic, data, "total", "Currently Used", "energy", "Wh", "Wh", EnergySensorFormat); break;
        case 2:
        sprintConfig(data);
        strcpy(topic, topic_config);
        break;
//        case 1: _registerAppliances(client); return true;
        default:
            LOG("UNKNOWN MQTT discovery %d", countdown);
            break;
    }
    if (strlen(topic) && strlen(data)) {
        client->publishDiscovery(topic, data);
    }
  }
  return true;
}

int parseUrl(char *host, char *user, char *pass, const char *localMqtt) {
    const char *at, *colon;
    int port = 1883;

    strcpy(user, "");
    strcpy(pass, "");
    if ((at = strchr(localMqtt, '@'))) {
        // username:password
        if ((colon = strchr(localMqtt, ':')) < at) {
            strncpy(user, localMqtt, colon-localMqtt);
            strncpy(pass, &colon[1], at-colon-1);
        }
        else {
            strncpy(user, localMqtt, at-localMqtt);
        }
        at++;
    }
    else {
        at = localMqtt;
    }
    // host:port
    if ((colon = strchr(at, ':'))) {
        strncpy(host, at, (colon-at));
        sscanf(&colon[1], "%d", &port);
    }
    else {
        strcpy(host, at);
    }
    return port;
}

void mqttSetup(){
    char host[64], user[64], pass[160];

    // migrate from user:pass@host:port to MQTT_HOST props?
    if (prefs()->isKey(PREFS_LOCAL_URL)) {
        memset(host, 0, 64);
        memset(user, 0, 64);
        memset(pass, 0, 160);
        int port = parseUrl(host, user, pass, prefs()->getString(PREFS_LOCAL_URL, "").c_str());
        prefsPutOrRemove(PREFS_MQTT_HOST, host);
        if (1883 != port) {
            prefs()->putInt(PREFS_MQTT_PORT, port);
        }
        else {
            prefs()->remove(PREFS_MQTT_PORT);
        }
        prefsPutOrRemove(PREFS_MQTT_USER, user);
        prefsPutOrRemove(PREFS_MQTT_PASS, pass);
        prefs()->remove(PREFS_LOCAL_URL);
    }

    if (prefs()->isKey(PREFS_MQTT_HOST)) {
        int32_t port = prefs()->getInt(PREFS_MQTT_PORT, 1883);
        memset(host, 0, 64);
        memset(user, 0, 64);
        memset(pass, 0, 160);
        prefs()->getString(PREFS_MQTT_HOST, host, 64);
        prefs()->getString(PREFS_MQTT_USER, user, 64);
        prefs()->getString(PREFS_MQTT_PASS, pass, 160);
        userClient = new SecureMqttClient(MQTT_TYPE_PLAIN_HA, host, port, user, pass);
    }

    if (prefs()->isKey(PREFS_DM_TYPE)) {
        int32_t caType = prefs()->getInt(PREFS_DM_TYPE, MQTT_TYPE_AZURE_IOT_HUB);
        int32_t port = prefs()->getInt(PREFS_DM_PORT, 8883);
        memset(host, 0, 64);
        memset(user, 0, 64);
        memset(pass, 0, 160);
        prefs()->getString(PREFS_DM_HOST, host, 64);
        prefs()->getString(PREFS_DM_USER, user, 64);
        prefs()->getString(PREFS_DM_PASS, pass, 160);
        dmClient = new SecureMqttClient(caType, host, port, user, pass);
    }
}

void mqttLoop() {
    if (_wifiConnected) {
        if (userClient) {
            userClient->loop();
        }

        if (dmClient) {
            dmClient->loop();
        }
    }
}
