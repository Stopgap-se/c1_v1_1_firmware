#ifndef _CRUTCH_H_
#define _CRUTCH_H_

#include "electricity_state.h"

void crutchOnLine(const char *line);
void crutchOnOverflow(const char *line, size_t len);
void crutchOnState(ElectricityStateProto *state);
void crutchRestart(const char *reason);

#endif // _CRUTCH_H_