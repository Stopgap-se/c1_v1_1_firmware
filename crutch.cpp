#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>

#include "electricity_state.h"

void crutchOnState(ElectricityStateProto *state) {
    LOG("%dW", state->metered.totalImportedPower);
}

