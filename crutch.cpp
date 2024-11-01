#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>
#include <time.h>

#include "electricity_state.h"
#include "obis_parser.h"
#include "prefs.h"
#include "tcp_server.h"
#include "web_server.h"

void crutchOnLine(const char *line) {
    tcpServerWrite(line, strlen(line));
    obisParseLine(line);
}

void crutchOnOverflow(const char *line, size_t len) {
    tcpServerWrite(line, strlen(line));
}

void crutchOnState(ElectricityStateProto *state) {
    struct tm tmNow;
    char message[1024];

    getLocalTime(&tmNow);
    state->timestamp = mktime(&tmNow);
    state->has_timestamp = true;

    esSprintfState(message, state, false);
    eventSourceSend(message, "state");
    LOG("state: %s %dW", state->metered.identifier, state->metered.totalImportedPower);
}

void crutchRestart(const char *reason) {
    char message[1024];
    prefs()->end();
    sprintf(message, "restart %s", reason);
    eventSourceSend(message, "status");
    eventSourceSend(message, "log");
    LOGS(message);
    eventSourceTeardown();
    delay(1000);
    ESP.restart();
}
