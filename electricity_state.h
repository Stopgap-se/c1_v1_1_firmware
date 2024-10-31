#ifndef _ELECTRICITY_STATE_H_
#define _ELECTRICITY_STATE_H_

#include "electricity_state.pb.h"

void esSet_dateTime(ElectricityStateProto *state, const char *dateTime);
void esSet_timestamp(ElectricityStateProto *state, uint32_t timestamp);
void esSet_hourChanged(ElectricityStateProto *state);
void esSet_identifier(ElectricityStateProto *state, const char *identifier);

void esReset(ElectricityStateProto *state);
int32_t esAdd_Wh(ElectricityStateProto *state, int32_t amount);
int32_t esAdd_Ph(ElectricityStateProto *state, int32_t amount);
int32_t esAdd_Rh(ElectricityStateProto *state, int32_t amount);
int32_t esAdd_Qh(ElectricityStateProto *state, int32_t amount);
int32_t esSet_Wh(ElectricityStateProto *state, int32_t amount);
int32_t esSet_Ph(ElectricityStateProto *state, int32_t amount);
int32_t esSet_Rh(ElectricityStateProto *state, int32_t amount);
int32_t esSet_Qh(ElectricityStateProto *state, int32_t amount);

void esSet_W(ElectricityStateProto *state, int32_t value);
void esSet_P(ElectricityStateProto *state, int32_t value);
void esSet_R(ElectricityStateProto *state, int32_t value);
void esSet_Q(ElectricityStateProto *state, int32_t value);
void esSet_L1(ElectricityStateProto *state, int32_t value);
void esSet_P1(ElectricityStateProto *state, int32_t value);
void esSet_R1(ElectricityStateProto *state, int32_t value);
void esSet_Q1(ElectricityStateProto *state, int32_t value);
void esSet_I1(ElectricityStateProto *state, float value);
void esSet_U1(ElectricityStateProto *state, float value);
void esSet_L2(ElectricityStateProto *state, int32_t value);
void esSet_P2(ElectricityStateProto *state, int32_t value);
void esSet_R2(ElectricityStateProto *state, int32_t value);
void esSet_Q2(ElectricityStateProto *state, int32_t value);
void esSet_I2(ElectricityStateProto *state, float value);
void esSet_U2(ElectricityStateProto *state, float value);
void esSet_L3(ElectricityStateProto *state, int32_t value);
void esSet_P3(ElectricityStateProto *state, int32_t value);
void esSet_R3(ElectricityStateProto *state, int32_t value);
void esSet_Q3(ElectricityStateProto *state, int32_t value);
void esSet_I3(ElectricityStateProto *state, float value);
void esSet_U3(ElectricityStateProto *state, float value);

void esSet_csW(ElectricityStateProto *state, float value);
void esSet_csL1(ElectricityStateProto *state, float value);
void esSet_csL2(ElectricityStateProto *state, float value);
void esSet_csL3(ElectricityStateProto *state, float value);
void esSet_csR(ElectricityStateProto *state, float value);
void esSet_csR1(ElectricityStateProto *state, float value);
void esSet_csR2(ElectricityStateProto *state, float value);
void esSet_csR3(ElectricityStateProto *state, float value);
void esSet_csBLINK(ElectricityStateProto *state, float value);

void esSet_dW(ElectricityStateProto *state, int32_t diff);
void esSet_dL1(ElectricityStateProto *state, int32_t diff);
void esSet_dL2(ElectricityStateProto *state, int32_t diff);
void esSet_dL3(ElectricityStateProto *state, int32_t diff);
void esSet_dR(ElectricityStateProto *state, int32_t diff);
void esSet_dR1(ElectricityStateProto *state, int32_t diff);
void esSet_dR2(ElectricityStateProto *state, int32_t diff);
void esSet_dR3(ElectricityStateProto *state, int32_t diff);
void esSet_dBLINK(ElectricityStateProto *state, int32_t diff);

void esSet_bW(ElectricityStateProto *state, int32_t value);
void esSet_bWh(ElectricityStateProto *state, uint32_t value);

void esAdd_detected(ElectricityStateProto *state, const char *deviceId, const char *profileId,
        const char *fromStateId, const char *toStateId, const char *transitionId,
        int32_t toState, int32_t algo, int32_t startedState, int actual, bool push, uint32_t Wh);

size_t esSprintfState(char *buf, ElectricityStateProto *state, bool pretty);

#endif // _ELECTRICITY_STATE_H_