#ifndef _DEVICE_API_H_
#define _DEVICE_API_H_

#include <Preferences.h>
#include <WiFi.h>

#define DEVICE_MODE_HAN   0x01
#define DEVICE_MODE_BLINK 0x02
#define DEVICE_MODE_RF    0x04
#define DEVICE_MODE_LED    0x08
#define DEVICE_MODE_DISPLAY 0x10

#define DEVICE_MODE_NEW   0x0F
#define DEVICE_MODE_HAN_PLUS_BLINK 0x0B
#define DEVICE_MODE_NAMES_MASK 0x07

#define DEVICE_STATE_IDLE 0
#define DEVICE_STATE_FOTA_HEADERS 1
#define DEVICE_STATE_FOTA_BODY 2
#define DEVICE_STATE_RESTARTING 3

#define ISO_8601_SIZE 28

static const char CurrentlyOne[] = {"CurrentlyOne"};

bool deviceIdle();

uint16_t rechargeDelay();

uint8_t getDeviceMode();

void setDeviceMode(uint8_t value);

const char *getDeviceModeNameSpec(uint8_t mode);
const char *getDeviceModeName();
const char * getDeviceId();

long getDeviceGmtOffset();
int getDeviceDaylightOffset();
const char *getDeviceDateTimeStrBy(struct tm timeinfo);
const char *getDeviceDateTimeStr();
uint32_t getDeviceSecondsSinceEpoch();

void deviceRestart(const char *reason, uint8_t level = 3);
void logHeapStack(const char *fn);

const char * deviceSetup();
int32_t deviceLoop();

const char * getResetReason(int reason);

int32_t deviceFota(WiFiClient *client, const char *boundary, const char *pMD5);

#endif // _DEVICE_API_H_