#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>

#include "electricity_state.h"
#include "han_reader.h"
#include "pins.h"
#include "prefs.h"

#define LINE_BUF_SIZE 256
#define TERM 0x21
#define NEWLINE 0x0A
#define SLASH 0x2F
#define DTS27_STZ351 0xCD
#define ISO_8601_SIZE 28

static void (*_stateCallback)(ElectricityStateProto *state);
static bool crcDisabled;
static float _tcCorr; // transformer constant
static uint32_t rtsLowMillis = 0, rtsBlockMillis = 0;

int strswap(char *str, char find, char replace) {
    int i = 0, n = 0;
    while (str[i]) {
        if (str[i] == find) {
            str[i] = replace;
            n++;
        }
        i++;
    }
    return n;
}

int32_t parseUnits(float valueF, const char *unit) {
    return strchr(unit, 0x6B) ? round(valueF*1000) : round(valueF);
}
float parseFloat(float valueF, const char *unit) {
    return strchr(unit, 0x6B) ? valueF*1000 : valueF;
}
int32_t toInteger(const char *rawValue) {
    float valueF;
    char unit[ISO_8601_SIZE];

    memset(unit, 0, ISO_8601_SIZE);
    if (strchr(rawValue, 0x2A)) { // *
        sscanf(rawValue, "%f*%s", &valueF, unit);
    }
    else {
        strncpy(unit, &rawValue[1], min(ISO_8601_SIZE-1, strchr(rawValue, 0x29)-rawValue-1));
    }
    return parseUnits(valueF*_tcCorr, unit);
}
float toFloat(const char *rawValue) {
    float valueF;
    sscanf(rawValue, "%f*", &valueF);
    return valueF;
}

void obisParseLine(const char *line) {
    static ElectricityStateProto state = ElectricityStateProto_init_default;
    static uint16_t crc;

    if (!strncmp(line, "1-0:", 4) && strchr(line, '(') && strchr(line, ')')) {
        int major, minor, tariff;
        char rawValue[LINE_BUF_SIZE];

        crc = updateCRC(crc, line, strlen(line));
        if (4 == sscanf(line, "1-0:%d.%d.%d(%s)", &major, &minor, &tariff, rawValue)) {
//            LOG("%d.%d.%d rawValue %s", major, minor, tariff, rawValue);
            switch (minor) {
            case 7:
                switch (major) {
                    case 1: esSet_W(&state, toInteger(rawValue)); break; // 1-0:1.7.0 Aktiv Effekt Uttag  Momentan trefaseffekt
                    case 2: esSet_P(&state, toInteger(rawValue)); break; // 1-0:2.7.0 Aktiv Effekt Inmatning Momentan trefaseffekt
                    case 3: esSet_R(&state, toInteger(rawValue)); break; // 1-0:3.7.0	Reaktiv Effekt Uttag	Momentan trefaseffekt	0000.000*kvar
                    case 4: esSet_Q(&state, toInteger(rawValue)); break; // 1-0:4.7.0	Reaktiv Effekt Inmatning	Momentan trefaseffekt	0000.309*kvar
                    case 21: esSet_L1(&state, toInteger(rawValue)); break; // 1-0:21.7.0  L1 Aktiv Effekt Uttag Momentan effekt
                    case 22: esSet_P1(&state, toInteger(rawValue)); break; // 1-0:22.7.0  L1 Aktiv Effekt Inmatning Momentan effekt
                    case 23: esSet_R1(&state, toInteger(rawValue)); break; // 1-0:23.7.0  L1 Reaktiv Effekt Uttag	Momentan effekt	0000.000*kvar
                    case 24: esSet_Q1(&state, toInteger(rawValue)); break; // 1-0:24.7.0	L1 Reaktiv Effekt Inmatning	Momentan effekt	0000.000*kvar
                    case 41: esSet_L2(&state, toInteger(rawValue)); break; // 1-0:41.7.0  L2 Aktiv Effekt Uttag Momentan effekt
                    case 42: esSet_P2(&state, toInteger(rawValue)); break; // 1-0:42.7.0  L2 Aktiv Effekt Inmatning Momentan effekt
                    case 43: esSet_R2(&state, toInteger(rawValue)); break; // 1-0:43.7.0  L2 Reaktiv Effekt Uttag	Momentan effekt	0000.000*kvar
                    case 44: esSet_Q2(&state, toInteger(rawValue)); break; // 1-0:44.7.0	Q2 Reaktiv Effekt Inmatning	Momentan effekt	0000.000*kvar
                    case 61: esSet_L3(&state, toInteger(rawValue)); break; // 1-0:61.7.0  L3 Aktiv Effekt Uttag Momentan effekt
                    case 62: esSet_P3(&state, toInteger(rawValue)); break; // 1-0:62.7.0  L3 Aktiv Effekt Inmatning Momentan effekt
                    case 63: esSet_R3(&state, toInteger(rawValue)); break; // 1-0:63.7.0  L3 Reaktiv Effekt Uttag	Momentan effekt	0000.000*kvar
                    case 64: esSet_Q3(&state, toInteger(rawValue)); break; // 1-0:64.7.0	L3 Reaktiv Effekt Inmatning	Momentan effekt	0000.000*kvar
                    case 31: esSet_I1(&state, toFloat(rawValue)*_tcCorr); break; // 1-0:31.7.0	L1 Fasström	Momentant RMS-värde	004.2*A
                    case 51: esSet_I2(&state, toFloat(rawValue)*_tcCorr); break; // 1-0:51.7.0	L2 Fasström	Momentant RMS-värde	001.6*A
                    case 71: esSet_I3(&state, toFloat(rawValue)*_tcCorr); break; // 1-0:71.7.0	L3 Fasström	Momentant RMS-värde	001.7*A
                    case 32: esSet_U1(&state, toFloat(rawValue)); break; // 1-0:32.7.0	L1 Fasspänning	Momentant RMS-värde	240.3*V
                    case 52: esSet_U2(&state, toFloat(rawValue)); break; // 1-0:52.7.0	L2 Fasspänning	Momentant RMS-värde	240.1*V
                    case 72: esSet_U3(&state, toFloat(rawValue)); break; // 1-0:72.7.0	L3 Fasspänning	Momentant RMS-värde	241.3*V
                }
                break;
            case 8:
                switch (major) {
                    case 1:
                        switch (tariff) {
                            case 0: // 1-0:1.8.0 Mätarställning Aktiv Energi Uttag.
                            case 1: // #67 1-0:1.8.1 day tariff
                                esSet_Wh(&state, toInteger(rawValue)); break;
                            default: // #67 1-0:1.8.2 night tariff
                                esAdd_Wh(&state, toInteger(rawValue)); break;
                        }
                        break;
                    case 2:
                        switch (tariff) {
                            case 0: // 1-0:2.8.0 Mätarställning Aktiv Energi Inmatning.
                            case 1: // #67 1-0:2.8.1 day tariff
                                esSet_Ph(&state, toInteger(rawValue)); break;
                        default: // #67 1-0:2.8.2 night tariff
                            esAdd_Ph(&state, toInteger(rawValue)); break;
                        }
                        break;
                    case 3:
                        switch (tariff) {
                            case 0: // 1-0:3.8.0 Mätarställning Reaktiv Energi Uttag		00000021.988*kvarh
                            case 1:
                                esSet_Rh(&state, toInteger(rawValue)); break;
                            default:
                                esAdd_Rh(&state, toInteger(rawValue)); break;
                        }
                        break;
                    case 4:
                        switch (tariff) {
                            case 0: // 1-0:4.8.0 Mätarställning Reaktiv Energi Inmatning		00001020.971*kvarh
                            case 1:
                                esSet_Qh(&state, toInteger(rawValue)); break;
                            default:
                                esAdd_Qh(&state, toInteger(rawValue)); break;
                        }
                        break;
                }
                break;
            }
        }
    }
    else if (!strncmp(line, "0-0:1.0.0(", 10)) {
        crc = updateCRC(crc, line, strlen(line));
        if (23 == strchr(line, ')') - line) { // 0-0:1.0.0(230622171700W)
            char dateTimeZ[ISO_8601_SIZE], tz[ISO_8601_SIZE];
            memset(dateTimeZ, 0, ISO_8601_SIZE);
            memset(tz, 0, ISO_8601_SIZE);
            snprintf(dateTimeZ, 5, "20%s", &line[10]);
            strcat(dateTimeZ, "-");
            strncat(dateTimeZ, &line[12], 2);
            strcat(dateTimeZ, "-");
            strncat(dateTimeZ, &line[14], 2);
            strcat(dateTimeZ, "T");
            strncat(dateTimeZ, &line[16], 2);
            strcat(dateTimeZ, ":");
            strncat(dateTimeZ, &line[18], 2);
            strcat(dateTimeZ, ":");
            strncat(dateTimeZ, &line[20], 2);
//            sprintf(tz, 0 <= gmtOffsetSec ? "+%02u%02u" : "-%02u%02u", abs(gmtOffsetSec) / 3600, abs(gmtOffsetSec % 3600)/60);
            strcat(dateTimeZ, "+0100"); // tz);
            state.meterType = MeterType_HAN;
            esSet_dateTime(&state, dateTimeZ);
        }
    }
    else if (strchr(line, '/')) {
        char *slash = strchr(line, '/');
        char *cr = strchr(slash, '\r');
        crc = updateCRC(0, slash, strlen(slash));
        esReset(&state);
        if (cr) {
            char identifier[LINE_BUF_SIZE];
            memset(identifier, 0, LINE_BUF_SIZE);
            strncpy(identifier, &slash[1], cr-slash-1);
            strswap(identifier, 0x5C, 0x5F); // replace \ with _
            esSet_identifier(&state, identifier);
        }
    }
    else if (strchr(line, DTS27_STZ351)) {
        // 0xCD is #58 hack for Shenzhen Star DTS27 type STZ351
        // 0D 0A B9 EA 75 EA CD 5C 32 4D 53 5A 33 31 34 58 0D 0A | ��u��\2MSZ314X
        char *slash = strchr(line, 0xB9);
        char *cr = strchr(slash, '\r');
        crc = updateCRC(0, slash, strlen(slash));
        // update CRC before patching:
        char *cd = strchr(line, DTS27_STZ351);
        cd[0] = SLASH;
        // END #58 hack for Shenzhen Star DTS27 type STZ351
        if (cr) {
            char identifier[LINE_BUF_SIZE];
            memset(identifier, 0, LINE_BUF_SIZE);
            strncpy(identifier, &cd[1], cr-cd-1);
            strswap(identifier, 0x5C, 0x5F); // replace \ with _
            esSet_identifier(&state, identifier);
        }
    }
    else if (strchr(line, '!')) {
        char calculated[8], *term = strchr(line, '!');
        crc = updateCRC(crc, line, term - line + 1);
        sprintf(calculated, "!%04X", crc);
        if (crcDisabled || !strncmp(term, calculated, 5)) {
            char dbg[128];
            sprintf(dbg, "/%s %s OK, %dW",
                state.metered.identifier, calculated, state.metered.totalImportedPower);
            LOGS(dbg);
            if (_stateCallback) {
                _stateCallback(&state);
            }
        }
        else {
            LOG("CRC calc %s != %s", calculated, term);
        }

        // disable RTS for /LGF5E360 and E350
        if (0 < rtsBlockMillis) {
            digitalWrite(GPIO_HAN_RTS, LOW);
            rtsLowMillis = millis();
            LOGS("HAN RTS disabled");
        }
    }
    else {
        crc = updateCRC(crc, line, strlen(line));
    }
}

void obisSetup(void (*stateCallback)(ElectricityStateProto *state)) {
    _stateCallback = stateCallback;
    crcDisabled = prefs()->getBool(PREFS_CRC_DIS);
    _tcCorr = 1000.0/prefs()->getInt(PREFS_HW_IMPS, 1000);
    rtsBlockMillis = prefs()->getUInt(PREFS_HW_RTS_MS, 0);
    LOG("obisSetup(CRC %s)", crcDisabled ? "disabled" : "verify");
}

void obisLoop() {
    const uint32_t ms = millis();
    // time to enable RTS again?
    if (rtsLowMillis && rtsLowMillis + rtsBlockMillis <= ms) {
        digitalWrite(GPIO_HAN_RTS, HIGH);
        rtsLowMillis = 0;
        LOGS("HAN RTS enabled");
    }
}