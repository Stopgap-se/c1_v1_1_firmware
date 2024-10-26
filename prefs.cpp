#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <Preferences.h>
#include <nvs_flash.h>

static Preferences _prefs;

void prefsEraseAll() {
  LOGS("Wiping ALL Preferences");
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init(); // initialize the NVS partition.
}

void prefsFactoryReset() {
    LOGS("FACTORY RESET...");
    // TODO: LED indication
    delay(1000);
    prefsEraseAll();
    LOGS("Rebooting...");
    delay(1000);
    ESP.restart();
}

int prefsPutOrRemove(const char *key, const char *value) {
    if (NULL != value && strlen(value)) {
        _prefs.putString(key, value);
        return strlen(value);
    }
    _prefs.remove(key);
    return 0;
}

void prefsSetup() {
    _prefs.begin("c1", false);
}

Preferences * prefs() {
    return &_prefs;
}
