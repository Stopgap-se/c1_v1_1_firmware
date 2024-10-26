#ifndef _PREFS_H_
#define _PREFS_H_

#include <Preferences.h>

const char PREFS_LOCAL_URL[] = {"mqtt"};
const char PREFS_LOCAL_NAME[] = {"name"};
const char PREFS_CLOUD_MODE[] = {"cloud"};
#define PREFS_LOCAL_URL_SIZE 64
const char PREFS_DEVICE_MODE[] = {"mode"};
const char PREFS_TELNET_HOST[] = {"telnet"};

const char PREFS_MQTT_HOST[] = {"mqHost"};
const char PREFS_MQTT_PORT[] = {"mqPort"};
const char PREFS_MQTT_USER[] = {"mqUser"};
const char PREFS_MQTT_PASS[] = {"mqPass"};
const char PREFS_DM_TYPE[] = {"dmType"};
const char PREFS_DM_HOST[] = {"dmHost"};
const char PREFS_DM_PORT[] = {"dmPort"};
const char PREFS_DM_USER[] = {"dmUser"};
const char PREFS_DM_PASS[] = {"dmPass"};

const char PREFS_DETECTION_INDEX[] = {"ddIdx"};

const char PREFS_WIFI_SSID[] = {"ssid"};
const char PREFS_WIFI_PASS[] = {"pass"};
const char PREFS_WIFI_TX_POWER[] = {"txPwr"};
const char PREFS_WIFI_AUTO_TX_POWER[] = {"autTX"};

const char PREFS_TIME_GMT_OFFSET[] = {"gmtOf"};
const char PREFS_TIME_DAYLIGHT_OFFSET[] = {"dayOf"};

const char PREFS_HW_BOOT[] = {"boot"};
const char PREFS_HW_RF[] = {"RF"};
const char PREFS_HW_HAN[] = {"HAN"};
const char PREFS_HW_HAN_CHANNEL[] = {"hanCh"};
const char PREFS_HW_BLINK[] = {"blink"};
const char PREFS_HW_IMPS[] = {"imps"};
const char PREFS_HW_MAX_W[] = {"maxW"};
const char PREFS_HW_RTS_MS[] = {"rts"};
const char PREFS_CRC_DIS[] = {"crc"};
const char PREFS_AUTO_FOTA_DIS[] = {"disFota"};
const char PREFS_PULLUP_DIS[] = {"pullup"};

void prefsEraseAll();
void prefsFactoryReset();
int prefsPutOrRemove(Preferences *prefs, const char *key, const char *value);
void prefsSetup();
Preferences * prefs();

#endif // _PREFS_H_