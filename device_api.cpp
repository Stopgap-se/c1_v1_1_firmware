#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <WiFi.h>

#include "prefs.h"

static char deviceIdReverse[14];

const char * getDeviceId() {
  return deviceIdReverse;
}

long getDeviceGmtOffset() {
    return prefs()->getInt(PREFS_TIME_GMT_OFFSET, 3600);
}
int getDeviceDaylightOffset() {
    return prefs()->getInt(PREFS_TIME_DAYLIGHT_OFFSET, 3600);
}

const char * deviceSetup() {
    uint8_t _macAddressBuf[8];
    WiFi.macAddress(_macAddressBuf);
    memset(deviceIdReverse, 0, 14);
    sprintf(deviceIdReverse, "%02x%02x%02x%02x%02x%02x", _macAddressBuf[5], _macAddressBuf[4], _macAddressBuf[3], _macAddressBuf[2], _macAddressBuf[1], _macAddressBuf[0]);
    LOG("deviceSetup(%s)", deviceIdReverse);

    return deviceIdReverse;
}

static const char NotApplicable[] = {"N/A"};
static const char *ResetReasons[] = {
    NotApplicable,
    "POWERON",
    NotApplicable,
    "SW",
    "OWDT",
    "DEEPSLEEP",
    "SDIO",
    "TG0WDT_SYS", // 7
    "TG1WDT_SYS",
    "RTCWDT_SYS",
    "INTRUSION",
    "TGWDT_CPU",
    "SW_CPU",
    "RTCWDT_CPU", // 13
    "EXT_CPU",
    "BROWN_OUT",
    "RTCWDT_RTC" // 16
};

const char * getResetReason(int reason) {
    if (0 <= reason && reason < 17) {
        return ResetReasons[reason];
    }
    return NotApplicable;
}