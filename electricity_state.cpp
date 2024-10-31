#define ENABLE_LOG4ARDUINO
#include <Arduino.h>
#include <log4arduino.h>

//#include "detected_device.h"
#include "electricity_state.h"

const char * getMeterType(MeterType meterType) {
    switch (meterType) {
    case MeterType_BLINK:
        return "BLINK";
    case MeterType_HAN:
        return "HAN";
    case MeterType_UNKNOWN:
    default:
        return "ALL";
    }
}

void esSet_dateTime(ElectricityStateProto *state, const char *dateTime) {
    strcpy(state->metered.dateTime, dateTime);
    state->metered.has_dateTime = true;
}
void esSet_timestamp(ElectricityStateProto *state, uint32_t timestamp) {
    state->timestamp = timestamp;
    state->has_timestamp = true;
}
void esSet_hourChanged(ElectricityStateProto *state) {
    state->hourChanged = true;
    state->has_hourChanged = true;
}
void esSet_identifier(ElectricityStateProto *state, const char *identifier) {
    strncpy(state->metered.identifier, identifier, sizeof(state->metered.identifier));
    state->metered.has_identifier = true;
}

void esReset(ElectricityStateProto *state) {
    ElectricityStateProto tmp = ElectricityStateProto_init_default;
    memcpy(state, &tmp, sizeof(ElectricityStateProto));
    esSet_identifier(state, "UNK5OWN");
}

int32_t esAdd_Wh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalImportedEnergy += amount;
    state->metered.has_totalImportedEnergy = true;
    return state->metered.totalImportedEnergy;
}
int32_t esAdd_Ph(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalExportedEnergy += amount;
    state->metered.has_totalExportedEnergy = true;
    return state->metered.totalExportedEnergy;
}
int32_t esAdd_Rh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalImportedReactiveEnergy += amount;
    state->metered.has_totalImportedReactiveEnergy = true;
    return state->metered.totalImportedReactiveEnergy;
}
int32_t esAdd_Qh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalExportedReactiveEnergy += amount;
    state->metered.has_totalExportedReactiveEnergy = true;
    return state->metered.totalExportedReactiveEnergy;
}
int32_t esSet_Wh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalImportedEnergy = amount;
    state->metered.has_totalImportedEnergy = true;
    return state->metered.totalImportedEnergy;
}
int32_t esSet_Ph(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalExportedEnergy = amount;
    state->metered.has_totalExportedEnergy = true;
    return state->metered.totalExportedEnergy;
}
int32_t esSet_Rh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalImportedReactiveEnergy = amount;
    state->metered.has_totalImportedReactiveEnergy = true;
    return state->metered.totalImportedReactiveEnergy;
}
int32_t esSet_Qh(ElectricityStateProto *state, int32_t amount) {
    state->metered.totalExportedReactiveEnergy = amount;
    state->metered.has_totalExportedReactiveEnergy = true;
    return state->metered.totalExportedReactiveEnergy;
}

void esSet_W(ElectricityStateProto *state, int32_t value) {
    state->metered.totalImportedPower = value;
    state->metered.has_totalImportedPower = true;
}
void esSet_P(ElectricityStateProto *state, int32_t value) {
    state->metered.totalExportedPower = value;
    state->metered.has_totalExportedPower = true;
}
void esSet_R(ElectricityStateProto *state, int32_t value) {
    state->metered.totalImportedReactivePower = value;
    state->metered.has_totalImportedReactivePower = true;
}
void esSet_Q(ElectricityStateProto *state, int32_t value) {
    state->metered.totalExportedReactivePower = value;
    state->metered.has_totalExportedReactivePower = true;
}
void esSet_L1(ElectricityStateProto *state, int32_t value) {
    state->metered.phase1ImportedPower = value;
    state->metered.has_phase1ImportedPower = true;
}
void esSet_P1(ElectricityStateProto *state, int32_t value) {
    state->metered.phase1ExportedPower = value;
    state->metered.has_phase1ExportedPower = true;
}
void esSet_R1(ElectricityStateProto *state, int32_t value) {
    state->metered.phase1ImportedReactivePower = value;
    state->metered.has_phase1ImportedReactivePower = true;
}
void esSet_Q1(ElectricityStateProto *state, int32_t value) {
    state->metered.phase1ExportedReactivePower = value;
    state->metered.has_phase1ExportedReactivePower = true;
}
void esSet_I1(ElectricityStateProto *state, float value) {
    state->metered.phase1Current = value;
    state->metered.has_phase1Current = true;
}
void esSet_U1(ElectricityStateProto *state, float value) {
    state->metered.phase1Voltage = value;
    state->metered.has_phase1Voltage = true;
}
void esSet_L2(ElectricityStateProto *state, int32_t value) {
    state->metered.phase2ImportedPower = value;
    state->metered.has_phase2ImportedPower = true;
}
void esSet_P2(ElectricityStateProto *state, int32_t value) {
    state->metered.phase2ExportedPower = value;
    state->metered.has_phase2ExportedPower = true;
}
void esSet_R2(ElectricityStateProto *state, int32_t value) {
    state->metered.phase2ImportedReactivePower = value;
    state->metered.has_phase2ImportedReactivePower = true;
}
void esSet_Q2(ElectricityStateProto *state, int32_t value) {
    state->metered.phase2ExportedReactivePower = value;
    state->metered.has_phase2ExportedReactivePower = true;
}
void esSet_I2(ElectricityStateProto *state, float value) {
    state->metered.phase2Current = value;
    state->metered.has_phase2Current = true;
}
void esSet_U2(ElectricityStateProto *state, float value) {
    state->metered.phase2Voltage = value;
    state->metered.has_phase2Voltage = true;
}
void esSet_L3(ElectricityStateProto *state, int32_t value) {
    state->metered.phase3ImportedPower = value;
    state->metered.has_phase3ImportedPower = true;
}
void esSet_P3(ElectricityStateProto *state, int32_t value) {
    state->metered.phase3ExportedPower = value;
    state->metered.has_phase3ExportedPower = true;
}
void esSet_R3(ElectricityStateProto *state, int32_t value) {
    state->metered.phase3ImportedReactivePower = value;
    state->metered.has_phase3ImportedReactivePower = true;
}
void esSet_Q3(ElectricityStateProto *state, int32_t value) {
    state->metered.phase3ExportedReactivePower = value;
    state->metered.has_phase3ExportedReactivePower = true;
}
void esSet_I3(ElectricityStateProto *state, float value) {
    state->metered.phase3Current = value;
    state->metered.has_phase3Current = true;
}
void esSet_U3(ElectricityStateProto *state, float value) {
    state->metered.phase3Voltage = value;
    state->metered.has_phase3Voltage = true;
}

void esSet_csW(ElectricityStateProto *state, float value) {
    state->scores.totalActive = value;
    state->has_scores = true;
    state->scores.has_totalActive = true;
}
void esSet_csR(ElectricityStateProto *state, float value) {
    state->scores.totalReactive = value;
    state->has_scores = true;
    state->scores.has_totalReactive = true;
}
void esSet_csL1(ElectricityStateProto *state, float value) {
    state->scores.phase1Active = value;
    state->has_scores = true;
    state->scores.has_phase1Active = true;
}
void esSet_csR1(ElectricityStateProto *state, float value) {
    state->scores.phase1Reactive = value;
    state->has_scores = true;
    state->scores.has_phase1Reactive = true;
}
void esSet_csL2(ElectricityStateProto *state, float value) {
    state->scores.phase2Active = value;
    state->has_scores = true;
    state->scores.has_phase2Active = true;
}
void esSet_csR2(ElectricityStateProto *state, float value) {
    state->scores.phase2Reactive = value;
    state->has_scores = true;
    state->scores.has_phase2Reactive = true;
}
void esSet_csL3(ElectricityStateProto *state, float value) {
    state->scores.phase3Active = value;
    state->has_scores = true;
    state->scores.has_phase3Active = true;
}
void esSet_csR3(ElectricityStateProto *state, float value) {
    state->scores.phase3Reactive = value;
    state->has_scores = true;
    state->scores.has_phase3Reactive = true;
}
void esSet_csBLINK(ElectricityStateProto *state, float value) {
    state->scores.blink = value;
    state->has_scores = true;
    state->scores.has_blink = true;
}

void esSet_dW(ElectricityStateProto *state, int32_t diff) {
    state->diffed.totalActive = diff;
    state->has_diffed = true;
    state->diffed.has_totalActive = true;
}
void esSet_dL1(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase1Active = diff;
    state->has_diffed = true;
    state->diffed.has_phase1Active = true;
}
void esSet_dL2(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase2Active = diff;
    state->has_diffed = true;
    state->diffed.has_phase2Active = true;
}
void esSet_dL3(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase3Active = diff;
    state->has_diffed = true;
    state->diffed.has_phase3Active = true;
}
void esSet_dR(ElectricityStateProto *state, int32_t diff) {
    state->diffed.totalReactive = diff;
    state->has_diffed = true;
    state->diffed.has_totalReactive = true;
}
void esSet_dR1(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase1Reactive = diff;
    state->has_diffed = true;
    state->diffed.has_phase1Reactive = true;
}
void esSet_dR2(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase2Reactive = diff;
    state->has_diffed = true;
    state->diffed.has_phase2Reactive = true;
}
void esSet_dR3(ElectricityStateProto *state, int32_t diff) {
    state->diffed.phase3Reactive = diff;
    state->has_diffed = true;
    state->diffed.has_phase3Reactive = true;
}
void esSet_dBLINK(ElectricityStateProto *state, int32_t diff) {
    state->diffed.blink = diff;
    state->has_diffed = true;
    state->diffed.has_blink = true;
}

void esSet_bW(ElectricityStateProto *state, int32_t value) {
    state->blink.power = value;
    state->has_blink = true;
}
void esSet_bWh(ElectricityStateProto *state, uint32_t value) {
    state->blink.energy = value;
    state->has_blink = true;
}

size_t esSprintfState(char *buf, ElectricityStateProto *state, bool pretty) {
    const char *prefix = pretty ? "\n\t" : "";
    size_t len = 0;

    len += sprintf(&buf[len], "{%s\"m\":\"%s\"", prefix, getMeterType(state->meterType));
    if (state->has_hourChanged && state->hourChanged) {
        len += sprintf(&buf[len], ",%s\"hc\":true", prefix);
    }
    // metered values:
    if (state->metered.has_totalImportedPower) {
        len += sprintf(&buf[len], ",%s\"W\":%d", prefix, state->metered.totalImportedPower);
    }
    if (state->metered.has_totalImportedEnergy) {
        len += sprintf(&buf[len], ",%s\"Wh\":%u", prefix, state->metered.totalImportedEnergy);
    }
    // blink:
    if (state->has_blink) {
        len += sprintf(&buf[len], ",%s\"bW\":%d", prefix, state->blink.power);
        len += sprintf(&buf[len], ",%s\"bWh\":%u", prefix, state->blink.energy);
    }

    if (state->metered.has_dateTime) {
        len += sprintf(&buf[len], ",%s\"t\":\"%s\"", prefix, state->metered.dateTime);
    }

    // metered values cont'd:
    if (state->metered.has_totalExportedEnergy) {
        len += sprintf(&buf[len], ",%s\"Ph\":%u", prefix, state->metered.totalExportedEnergy);
    }
    if (state->metered.has_totalImportedReactiveEnergy) {
        len += sprintf(&buf[len], ",%s\"Rh\":%u", prefix, state->metered.totalImportedReactiveEnergy);
    }
    if (state->metered.has_totalExportedReactiveEnergy) {
        len += sprintf(&buf[len], ",%s\"Qh\":%u", prefix, state->metered.totalExportedReactiveEnergy);
    }

    if (state->metered.has_totalExportedPower) {
        len += sprintf(&buf[len], ",%s\"P\":%d", prefix, state->metered.totalExportedPower);
    }
    if (state->metered.has_totalImportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"R\":%d", prefix, state->metered.totalImportedReactivePower);
    }
    if (state->metered.has_totalExportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"Q\":%d", prefix, state->metered.totalExportedReactivePower);
    }

    if (state->metered.has_phase1ImportedPower) {
        len += sprintf(&buf[len], ",%s\"L1\":%ld", prefix, state->metered.phase1ImportedPower);
    }
    if (state->metered.has_phase1ExportedPower) {
        len += sprintf(&buf[len], ",%s\"P1\":%ld", prefix, state->metered.phase1ExportedPower);
    }
    if (state->metered.has_phase1ImportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"R1\":%ld", prefix, state->metered.phase1ImportedReactivePower);
    }
    if (state->metered.has_phase1ExportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"Q1\":%ld", prefix, state->metered.phase1ExportedReactivePower);
    }
    if (state->metered.has_phase1Current) {
        len += sprintf(&buf[len], ",%s\"I1\":%.3f", prefix, state->metered.phase1Current);
    }
    if (state->metered.has_phase1Voltage) {
        len += sprintf(&buf[len], ",%s\"U1\":%.3f", prefix, state->metered.phase1Voltage);
    }

    if (state->metered.has_phase2ImportedPower) {
        len += sprintf(&buf[len], ",%s\"L2\":%ld", prefix, state->metered.phase2ImportedPower);
    }
    if (state->metered.has_phase2ExportedPower) {
        len += sprintf(&buf[len], ",%s\"P2\":%ld", prefix, state->metered.phase2ExportedPower);
    }
    if (state->metered.has_phase2ImportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"R2\":%ld", prefix, state->metered.phase2ImportedReactivePower);
    }
    if (state->metered.has_phase2ExportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"Q2\":%ld", prefix, state->metered.phase2ExportedReactivePower);
    }
    if (state->metered.has_phase2Current) {
        len += sprintf(&buf[len], ",%s\"I2\":%.3f", prefix, state->metered.phase2Current);
    }
    if (state->metered.has_phase2Voltage) {
        len += sprintf(&buf[len], ",%s\"U2\":%.3f", prefix, state->metered.phase2Voltage);
    }

    if (state->metered.has_phase3ImportedPower) {
        len += sprintf(&buf[len], ",%s\"L3\":%ld", prefix, state->metered.phase3ImportedPower);
    }
    if (state->metered.has_phase3ExportedPower) {
        len += sprintf(&buf[len], ",%s\"P3\":%ld", prefix, state->metered.phase3ExportedPower);
    }
    if (state->metered.has_phase3ImportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"R3\":%ld", prefix, state->metered.phase3ImportedReactivePower);
    }
    if (state->metered.has_phase3ExportedReactivePower) {
        len += sprintf(&buf[len], ",%s\"Q3\":%ld", prefix, state->metered.phase3ExportedReactivePower);
    }
    if (state->metered.has_phase3Current) {
        len += sprintf(&buf[len], ",%s\"I3\":%.3f", prefix, state->metered.phase3Current);
    }
    if (state->metered.has_phase3Voltage) {
        len += sprintf(&buf[len], ",%s\"U3\":%.3f", prefix, state->metered.phase3Voltage);
    }
    if (state->has_timestamp) {
        len += sprintf(&buf[len], ",%s\"sec\":%u", prefix, state->timestamp);
    }

    // event differences:
//    if (state->has_diffed) {
//        if (state->diffed.has_totalActive) {
//            len += sprintf(&buf[len], ",%s\"dW\":%d", prefix, state->diffed.totalActive);
//        }
//        if (state->diffed.has_totalReactive) {
//            len += sprintf(&buf[len], ",%s\"dR\":%d", prefix, state->diffed.totalReactive);
//        }
//        if (state->diffed.has_phase1Active) {
//            len += sprintf(&buf[len], ",%s\"dL1\":%d", prefix, state->diffed.phase1Active);
//        }
//        if (state->diffed.has_phase1Reactive) {
//            len += sprintf(&buf[len], ",%s\"dR1\":%d", prefix, state->diffed.phase1Reactive);
//        }
//        if (state->diffed.has_phase2Active) {
//            len += sprintf(&buf[len], ",%s\"dL2\":%d", prefix, state->diffed.phase2Active);
//        }
//        if (state->diffed.has_phase2Reactive) {
//            len += sprintf(&buf[len], ",%s\"dR2\":%d", prefix, state->diffed.phase2Reactive);
//        }
//        if (state->diffed.has_phase3Active) {
//            len += sprintf(&buf[len], ",%s\"dL3\":%d", prefix, state->diffed.phase3Active);
//        }
//        if (state->diffed.has_phase3Reactive) {
//            len += sprintf(&buf[len], ",%s\"dR3\":%d", prefix, state->diffed.phase3Reactive);
//        }
//        if (state->diffed.has_blink) {
//            len += sprintf(&buf[len], ",%s\"dBLINK\":%d", prefix, state->diffed.blink);
//        }
//    }

    len += sprintf(&buf[len], "%s}", prefix);
    return len;
}
