/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.7 */

#ifndef PB_ELECTRICITY_STATE_PB_H_INCLUDED
#define PB_ELECTRICITY_STATE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _MeterType {
    MeterType_UNKNOWN = 0,
    MeterType_BLINK = 1,
    MeterType_HAN = 2
} MeterType;

/* Struct definitions */
typedef struct _MinMaxEntryProto {
    int32_t min;
    int32_t max;
    int32_t first;
} MinMaxEntryProto;

typedef struct _MinMaxCompositeProto {
    MinMaxEntryProto totalActive;
    MinMaxEntryProto phase1Active;
    MinMaxEntryProto phase2Active;
    MinMaxEntryProto phase3Active;
    MinMaxEntryProto phase1Current;
    MinMaxEntryProto phase2Current;
    MinMaxEntryProto phase3Current;
    MinMaxEntryProto phase1Voltage;
    MinMaxEntryProto phase2Voltage;
    MinMaxEntryProto phase3Voltage;
} MinMaxCompositeProto;

typedef struct _DetectedStateProto {
    char deviceId[21];
    bool has_state;
    int32_t state;
    bool has_algo;
    int32_t algo;
    bool has_profileId;
    char profileId[21];
    bool has_fromStateId;
    char fromStateId[21];
    bool has_toStateId;
    char toStateId[21];
    bool has_transitionId;
    char transitionId[21];
    bool has_actual;
    int32_t actual; /* actual diff */
    bool has_startedState;
    int32_t startedState; /*  */
    bool has_push;
    bool push;
    bool has_wh;
    uint32_t wh;
} DetectedStateProto;

typedef struct _ChangeScoresProto {
    bool has_totalActive;
    float totalActive;
    bool has_totalReactive;
    float totalReactive;
    bool has_phase1Active;
    float phase1Active;
    bool has_phase1Reactive;
    float phase1Reactive;
    bool has_phase2Active;
    float phase2Active;
    bool has_phase2Reactive;
    float phase2Reactive;
    bool has_phase3Active;
    float phase3Active;
    bool has_phase3Reactive;
    float phase3Reactive;
    bool has_blink;
    float blink;
} ChangeScoresProto;

typedef struct _DiffedPowersProto {
    bool has_totalActive;
    int32_t totalActive;
    bool has_totalReactive;
    int32_t totalReactive;
    bool has_phase1Active;
    int32_t phase1Active;
    bool has_phase1Reactive;
    int32_t phase1Reactive;
    bool has_phase2Active;
    int32_t phase2Active;
    bool has_phase2Reactive;
    int32_t phase2Reactive;
    bool has_phase3Active;
    int32_t phase3Active;
    bool has_phase3Reactive;
    int32_t phase3Reactive;
    bool has_blink;
    int32_t blink;
} DiffedPowersProto;

typedef struct _BlinkFieldsProto {
    int32_t power; /* blink frequency */
    uint32_t energy; /* blink count */
} BlinkFieldsProto;

typedef struct _HanFieldsProto {
    bool has_dateTime;
    char dateTime[26]; /* 1.0.0 */
    bool has_totalImportedPower;
    int32_t totalImportedPower; /* 1.7.0 W */
    bool has_totalImportedEnergy;
    uint32_t totalImportedEnergy; /* 1.8.0 Wh */
    bool has_totalExportedPower;
    int32_t totalExportedPower; /* 2.7.0 P */
    bool has_totalExportedEnergy;
    uint32_t totalExportedEnergy; /* 2.8.0 Ph */
    bool has_totalImportedReactivePower;
    int32_t totalImportedReactivePower; /* 3.7.0 R */
    bool has_totalImportedReactiveEnergy;
    uint32_t totalImportedReactiveEnergy; /* 3.8.0 Rh */
    bool has_totalExportedReactivePower;
    int32_t totalExportedReactivePower; /* 4.7.0 Q */
    bool has_totalExportedReactiveEnergy;
    uint32_t totalExportedReactiveEnergy; /* 4.8.0 Qh */
    bool has_phase1ImportedPower;
    int32_t phase1ImportedPower; /* 21.7.0 L1 */
    bool has_phase1ExportedPower;
    int32_t phase1ExportedPower; /* 22.7.0 P1 */
    bool has_phase1ImportedReactivePower;
    int32_t phase1ImportedReactivePower; /* 23.7.0 R1 */
    bool has_phase1ExportedReactivePower;
    int32_t phase1ExportedReactivePower; /* 24.7.0 Q1 */
    bool has_phase1Current;
    float phase1Current; /* 31.7.0 I1 */
    bool has_phase1Voltage;
    float phase1Voltage; /* 32.7.0 U1 */
    bool has_phase2ImportedPower;
    int32_t phase2ImportedPower; /* 41.7.0 L2 */
    bool has_phase2ExportedPower;
    int32_t phase2ExportedPower; /* 42.7.0 P2 */
    bool has_phase2ImportedReactivePower;
    int32_t phase2ImportedReactivePower; /* 43.7.0 R2 */
    bool has_phase2ExportedReactivePower;
    int32_t phase2ExportedReactivePower; /* 44.7.0 Q2 */
    bool has_phase2Current;
    float phase2Current; /* 51.7.0 I2 */
    bool has_phase2Voltage;
    float phase2Voltage; /* 52.7.0 U2 */
    bool has_phase3ImportedPower;
    int32_t phase3ImportedPower; /* 61.7.0 L3 */
    bool has_phase3ExportedPower;
    int32_t phase3ExportedPower; /* 62.7.0 P3 */
    bool has_phase3ImportedReactivePower;
    int32_t phase3ImportedReactivePower; /* 63.7.0 R3 */
    bool has_phase3ExportedReactivePower;
    int32_t phase3ExportedReactivePower; /* 64.7.0 Q3 */
    bool has_phase3Current;
    float phase3Current; /* 71.7.0 I3 */
    bool has_phase3Voltage;
    float phase3Voltage; /* 72.7.0 U3 */
    bool has_identifier;
    char identifier[26];
} HanFieldsProto;

typedef struct _ElectricityStateProto {
    MeterType meterType;
    bool has_timestamp;
    uint32_t timestamp;
    HanFieldsProto metered;
    bool has_diffed;
    DiffedPowersProto diffed; /* event difference */
    bool has_scores;
    ChangeScoresProto scores; /* change scores */
    bool has_blink;
    BlinkFieldsProto blink;
    bool has_hourChanged;
    bool hourChanged;
    pb_size_t detected_count;
    DetectedStateProto detected[8];
    bool has_minMax;
    MinMaxCompositeProto minMax;
} ElectricityStateProto;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _MeterType_MIN MeterType_UNKNOWN
#define _MeterType_MAX MeterType_HAN
#define _MeterType_ARRAYSIZE ((MeterType)(MeterType_HAN+1))








#define ElectricityStateProto_meterType_ENUMTYPE MeterType


/* Initializer values for message structs */
#define MinMaxEntryProto_init_default            {0, 0, 0}
#define MinMaxCompositeProto_init_default        {MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default, MinMaxEntryProto_init_default}
#define DetectedStateProto_init_default          {"", false, 0, false, 0, false, "", false, "", false, "", false, "", false, 0, false, 0, false, 0, false, 0}
#define ChangeScoresProto_init_default           {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define DiffedPowersProto_init_default           {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define BlinkFieldsProto_init_default            {0, 0}
#define HanFieldsProto_init_default              {false, "", false, 0, false, 0u, false, 0, false, 0u, false, 0, false, 0u, false, 0, false, 0u, false, 0, false, 0, false, 0, false, 0, false, 0.0f, false, 0.0f, false, 0, false, 0, false, 0, false, 0, false, 0.0f, false, 0.0f, false, 0, false, 0, false, 0, false, 0, false, 0.0f, false, 0.0f, false, ""}
#define ElectricityStateProto_init_default       {_MeterType_MIN, false, 0, HanFieldsProto_init_default, false, DiffedPowersProto_init_default, false, ChangeScoresProto_init_default, false, BlinkFieldsProto_init_default, false, 0, 0, {DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default, DetectedStateProto_init_default}, false, MinMaxCompositeProto_init_default}
#define MinMaxEntryProto_init_zero               {0, 0, 0}
#define MinMaxCompositeProto_init_zero           {MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero, MinMaxEntryProto_init_zero}
#define DetectedStateProto_init_zero             {"", false, 0, false, 0, false, "", false, "", false, "", false, "", false, 0, false, 0, false, 0, false, 0}
#define ChangeScoresProto_init_zero              {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define DiffedPowersProto_init_zero              {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0}
#define BlinkFieldsProto_init_zero               {0, 0}
#define HanFieldsProto_init_zero                 {false, "", false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, ""}
#define ElectricityStateProto_init_zero          {_MeterType_MIN, false, 0, HanFieldsProto_init_zero, false, DiffedPowersProto_init_zero, false, ChangeScoresProto_init_zero, false, BlinkFieldsProto_init_zero, false, 0, 0, {DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero, DetectedStateProto_init_zero}, false, MinMaxCompositeProto_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define MinMaxEntryProto_min_tag                 1
#define MinMaxEntryProto_max_tag                 2
#define MinMaxEntryProto_first_tag               3
#define MinMaxCompositeProto_totalActive_tag     1
#define MinMaxCompositeProto_phase1Active_tag    2
#define MinMaxCompositeProto_phase2Active_tag    3
#define MinMaxCompositeProto_phase3Active_tag    4
#define MinMaxCompositeProto_phase1Current_tag   5
#define MinMaxCompositeProto_phase2Current_tag   6
#define MinMaxCompositeProto_phase3Current_tag   7
#define MinMaxCompositeProto_phase1Voltage_tag   8
#define MinMaxCompositeProto_phase2Voltage_tag   9
#define MinMaxCompositeProto_phase3Voltage_tag   10
#define DetectedStateProto_deviceId_tag          1
#define DetectedStateProto_state_tag             2
#define DetectedStateProto_algo_tag              3
#define DetectedStateProto_profileId_tag         4
#define DetectedStateProto_fromStateId_tag       5
#define DetectedStateProto_toStateId_tag         6
#define DetectedStateProto_transitionId_tag      7
#define DetectedStateProto_actual_tag            8
#define DetectedStateProto_startedState_tag      9
#define DetectedStateProto_push_tag              10
#define DetectedStateProto_wh_tag                11
#define ChangeScoresProto_totalActive_tag        1
#define ChangeScoresProto_totalReactive_tag      2
#define ChangeScoresProto_phase1Active_tag       3
#define ChangeScoresProto_phase1Reactive_tag     4
#define ChangeScoresProto_phase2Active_tag       5
#define ChangeScoresProto_phase2Reactive_tag     6
#define ChangeScoresProto_phase3Active_tag       7
#define ChangeScoresProto_phase3Reactive_tag     8
#define ChangeScoresProto_blink_tag              9
#define DiffedPowersProto_totalActive_tag        1
#define DiffedPowersProto_totalReactive_tag      2
#define DiffedPowersProto_phase1Active_tag       3
#define DiffedPowersProto_phase1Reactive_tag     4
#define DiffedPowersProto_phase2Active_tag       5
#define DiffedPowersProto_phase2Reactive_tag     6
#define DiffedPowersProto_phase3Active_tag       7
#define DiffedPowersProto_phase3Reactive_tag     8
#define DiffedPowersProto_blink_tag              9
#define BlinkFieldsProto_power_tag               1
#define BlinkFieldsProto_energy_tag              2
#define HanFieldsProto_dateTime_tag              1
#define HanFieldsProto_totalImportedPower_tag    2
#define HanFieldsProto_totalImportedEnergy_tag   3
#define HanFieldsProto_totalExportedPower_tag    4
#define HanFieldsProto_totalExportedEnergy_tag   5
#define HanFieldsProto_totalImportedReactivePower_tag 6
#define HanFieldsProto_totalImportedReactiveEnergy_tag 7
#define HanFieldsProto_totalExportedReactivePower_tag 8
#define HanFieldsProto_totalExportedReactiveEnergy_tag 9
#define HanFieldsProto_phase1ImportedPower_tag   10
#define HanFieldsProto_phase1ExportedPower_tag   11
#define HanFieldsProto_phase1ImportedReactivePower_tag 12
#define HanFieldsProto_phase1ExportedReactivePower_tag 13
#define HanFieldsProto_phase1Current_tag         14
#define HanFieldsProto_phase1Voltage_tag         15
#define HanFieldsProto_phase2ImportedPower_tag   16
#define HanFieldsProto_phase2ExportedPower_tag   17
#define HanFieldsProto_phase2ImportedReactivePower_tag 18
#define HanFieldsProto_phase2ExportedReactivePower_tag 19
#define HanFieldsProto_phase2Current_tag         20
#define HanFieldsProto_phase2Voltage_tag         21
#define HanFieldsProto_phase3ImportedPower_tag   22
#define HanFieldsProto_phase3ExportedPower_tag   23
#define HanFieldsProto_phase3ImportedReactivePower_tag 24
#define HanFieldsProto_phase3ExportedReactivePower_tag 25
#define HanFieldsProto_phase3Current_tag         26
#define HanFieldsProto_phase3Voltage_tag         27
#define HanFieldsProto_identifier_tag            28
#define ElectricityStateProto_meterType_tag      1
#define ElectricityStateProto_timestamp_tag      2
#define ElectricityStateProto_metered_tag        3
#define ElectricityStateProto_diffed_tag         4
#define ElectricityStateProto_scores_tag         5
#define ElectricityStateProto_blink_tag          6
#define ElectricityStateProto_hourChanged_tag    7
#define ElectricityStateProto_detected_tag       8
#define ElectricityStateProto_minMax_tag         9

/* Struct field encoding specification for nanopb */
#define MinMaxEntryProto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, SINT32,   min,               1) \
X(a, STATIC,   REQUIRED, SINT32,   max,               2) \
X(a, STATIC,   REQUIRED, SINT32,   first,             3)
#define MinMaxEntryProto_CALLBACK NULL
#define MinMaxEntryProto_DEFAULT NULL

#define MinMaxCompositeProto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, MESSAGE,  totalActive,       1) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase1Active,      2) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase2Active,      3) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase3Active,      4) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase1Current,     5) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase2Current,     6) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase3Current,     7) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase1Voltage,     8) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase2Voltage,     9) \
X(a, STATIC,   REQUIRED, MESSAGE,  phase3Voltage,    10)
#define MinMaxCompositeProto_CALLBACK NULL
#define MinMaxCompositeProto_DEFAULT NULL
#define MinMaxCompositeProto_totalActive_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase1Active_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase2Active_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase3Active_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase1Current_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase2Current_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase3Current_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase1Voltage_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase2Voltage_MSGTYPE MinMaxEntryProto
#define MinMaxCompositeProto_phase3Voltage_MSGTYPE MinMaxEntryProto

#define DetectedStateProto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, STRING,   deviceId,          1) \
X(a, STATIC,   OPTIONAL, SINT32,   state,             2) \
X(a, STATIC,   OPTIONAL, SINT32,   algo,              3) \
X(a, STATIC,   OPTIONAL, STRING,   profileId,         4) \
X(a, STATIC,   OPTIONAL, STRING,   fromStateId,       5) \
X(a, STATIC,   OPTIONAL, STRING,   toStateId,         6) \
X(a, STATIC,   OPTIONAL, STRING,   transitionId,      7) \
X(a, STATIC,   OPTIONAL, SINT32,   actual,            8) \
X(a, STATIC,   OPTIONAL, SINT32,   startedState,      9) \
X(a, STATIC,   OPTIONAL, BOOL,     push,             10) \
X(a, STATIC,   OPTIONAL, UINT32,   wh,               11)
#define DetectedStateProto_CALLBACK NULL
#define DetectedStateProto_DEFAULT NULL

#define ChangeScoresProto_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, FLOAT,    totalActive,       1) \
X(a, STATIC,   OPTIONAL, FLOAT,    totalReactive,     2) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase1Active,      3) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase1Reactive,    4) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase2Active,      5) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase2Reactive,    6) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase3Active,      7) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase3Reactive,    8) \
X(a, STATIC,   OPTIONAL, FLOAT,    blink,             9)
#define ChangeScoresProto_CALLBACK NULL
#define ChangeScoresProto_DEFAULT NULL

#define DiffedPowersProto_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, SINT32,   totalActive,       1) \
X(a, STATIC,   OPTIONAL, SINT32,   totalReactive,     2) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1Active,      3) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1Reactive,    4) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2Active,      5) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2Reactive,    6) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3Active,      7) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3Reactive,    8) \
X(a, STATIC,   OPTIONAL, SINT32,   blink,             9)
#define DiffedPowersProto_CALLBACK NULL
#define DiffedPowersProto_DEFAULT NULL

#define BlinkFieldsProto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, SINT32,   power,             1) \
X(a, STATIC,   REQUIRED, UINT32,   energy,            2)
#define BlinkFieldsProto_CALLBACK NULL
#define BlinkFieldsProto_DEFAULT NULL

#define HanFieldsProto_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, STRING,   dateTime,          1) \
X(a, STATIC,   OPTIONAL, SINT32,   totalImportedPower,   2) \
X(a, STATIC,   OPTIONAL, UINT32,   totalImportedEnergy,   3) \
X(a, STATIC,   OPTIONAL, SINT32,   totalExportedPower,   4) \
X(a, STATIC,   OPTIONAL, UINT32,   totalExportedEnergy,   5) \
X(a, STATIC,   OPTIONAL, SINT32,   totalImportedReactivePower,   6) \
X(a, STATIC,   OPTIONAL, UINT32,   totalImportedReactiveEnergy,   7) \
X(a, STATIC,   OPTIONAL, SINT32,   totalExportedReactivePower,   8) \
X(a, STATIC,   OPTIONAL, UINT32,   totalExportedReactiveEnergy,   9) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1ImportedPower,  10) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1ExportedPower,  11) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1ImportedReactivePower,  12) \
X(a, STATIC,   OPTIONAL, SINT32,   phase1ExportedReactivePower,  13) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase1Current,    14) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase1Voltage,    15) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2ImportedPower,  16) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2ExportedPower,  17) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2ImportedReactivePower,  18) \
X(a, STATIC,   OPTIONAL, SINT32,   phase2ExportedReactivePower,  19) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase2Current,    20) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase2Voltage,    21) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3ImportedPower,  22) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3ExportedPower,  23) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3ImportedReactivePower,  24) \
X(a, STATIC,   OPTIONAL, SINT32,   phase3ExportedReactivePower,  25) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase3Current,    26) \
X(a, STATIC,   OPTIONAL, FLOAT,    phase3Voltage,    27) \
X(a, STATIC,   OPTIONAL, STRING,   identifier,       28)
#define HanFieldsProto_CALLBACK NULL
#define HanFieldsProto_DEFAULT (const pb_byte_t*)"\x10\x00\x18\x00\x20\x00\x28\x00\x30\x00\x38\x00\x40\x00\x48\x00\x50\x00\x58\x00\x60\x00\x68\x00\x75\x00\x00\x00\x00\x7d\x00\x00\x00\x00\x80\x01\x00\x88\x01\x00\x90\x01\x00\x98\x01\x00\xa5\x01\x00\x00\x00\x00\xad\x01\x00\x00\x00\x00\xb0\x01\x00\xb8\x01\x00\xc0\x01\x00\xc8\x01\x00\xd5\x01\x00\x00\x00\x00\xdd\x01\x00\x00\x00\x00\x00"

#define ElectricityStateProto_FIELDLIST(X, a) \
X(a, STATIC,   REQUIRED, UENUM,    meterType,         1) \
X(a, STATIC,   OPTIONAL, UINT32,   timestamp,         2) \
X(a, STATIC,   REQUIRED, MESSAGE,  metered,           3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  diffed,            4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  scores,            5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  blink,             6) \
X(a, STATIC,   OPTIONAL, BOOL,     hourChanged,       7) \
X(a, STATIC,   REPEATED, MESSAGE,  detected,          8) \
X(a, STATIC,   OPTIONAL, MESSAGE,  minMax,            9)
#define ElectricityStateProto_CALLBACK NULL
#define ElectricityStateProto_DEFAULT NULL
#define ElectricityStateProto_metered_MSGTYPE HanFieldsProto
#define ElectricityStateProto_diffed_MSGTYPE DiffedPowersProto
#define ElectricityStateProto_scores_MSGTYPE ChangeScoresProto
#define ElectricityStateProto_blink_MSGTYPE BlinkFieldsProto
#define ElectricityStateProto_detected_MSGTYPE DetectedStateProto
#define ElectricityStateProto_minMax_MSGTYPE MinMaxCompositeProto

extern const pb_msgdesc_t MinMaxEntryProto_msg;
extern const pb_msgdesc_t MinMaxCompositeProto_msg;
extern const pb_msgdesc_t DetectedStateProto_msg;
extern const pb_msgdesc_t ChangeScoresProto_msg;
extern const pb_msgdesc_t DiffedPowersProto_msg;
extern const pb_msgdesc_t BlinkFieldsProto_msg;
extern const pb_msgdesc_t HanFieldsProto_msg;
extern const pb_msgdesc_t ElectricityStateProto_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define MinMaxEntryProto_fields &MinMaxEntryProto_msg
#define MinMaxCompositeProto_fields &MinMaxCompositeProto_msg
#define DetectedStateProto_fields &DetectedStateProto_msg
#define ChangeScoresProto_fields &ChangeScoresProto_msg
#define DiffedPowersProto_fields &DiffedPowersProto_msg
#define BlinkFieldsProto_fields &BlinkFieldsProto_msg
#define HanFieldsProto_fields &HanFieldsProto_msg
#define ElectricityStateProto_fields &ElectricityStateProto_msg

/* Maximum encoded size of messages (where known) */
#define BlinkFieldsProto_size                    12
#define ChangeScoresProto_size                   45
#define DetectedStateProto_size                  142
#define DiffedPowersProto_size                   54
#define ElectricityStateProto_size               1710
#define HanFieldsProto_size                      217
#define MinMaxCompositeProto_size                200
#define MinMaxEntryProto_size                    18

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
