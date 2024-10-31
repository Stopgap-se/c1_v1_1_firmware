#ifndef _OBIS_PARSER_H_
#define _OBIS_PARSER_H_

#include "electricity_state.h"

void obisParseLine(const char *line);

void obisSetup(void (*stateCallback)(ElectricityStateProto *state));
void obisLoop();

#endif // _OBIS_PARSER_H_