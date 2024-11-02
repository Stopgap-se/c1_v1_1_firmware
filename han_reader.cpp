#define ENABLE_LOG4ARDUINO
#include <log4arduino.h>

#include "han_reader.h"
#include "pins.h"
#include "prefs.h"

#define HAN_BUF_SIZE 0x800
#define HAN_BUF_MASK 0x7FF
#define LINE_BUF_SIZE 256

static char hanBuf[HAN_BUF_SIZE];
static char hanPrefix[5];
static volatile uint16_t hanRead = 0xF000, hanWritten = 0xF000;
static int32_t gmtOffsetSec;
static void (*_lineCallback)(const char *line);
static void (*_overflowCallback)(const char *line, size_t len);

uint16_t _updateCRC16(uint16_t crc, uint8_t data) {
  crc ^= data;
  for (int i = 0; i < 8; ++i) {
    if (crc & 1) {
        crc >>= 1;
        crc ^= 0xA001;
    }
    else {
        crc >>= 1;
    }
  }
  return crc;
}

uint16_t updateCRC(uint16_t crc, const char *data, size_t len) {
    for (int i = 0; i < len; i++) {
        crc = _updateCRC16(crc, (uint8_t) data[i]);
    }
    return crc;
}

void _hanProcess()
{
    const uint32_t ms = millis();
    static uint32_t lastConnectAttempt = 0, lastDataReceived = 0;

    if (hanRead != hanWritten) {
        char lineBuf[LINE_BUF_SIZE];
        memset(lineBuf, 0, LINE_BUF_SIZE);
        uint16_t i = hanRead;
        lastDataReceived = ms;

        while (i != hanWritten) {
            const uint16_t lineOff = i - hanRead;
            if (lineOff < LINE_BUF_SIZE-1) {
                lineBuf[lineOff] = hanBuf[i & HAN_BUF_MASK];
                if ('\n' == hanBuf[i & HAN_BUF_MASK]) {

                    hanRead = i+1;
                    if (_lineCallback) {
                        _lineCallback(lineBuf);
                    }
                    else {
                        LOGS(lineBuf);
                    }
                    memset(lineBuf, 0, LINE_BUF_SIZE);
                }
            }
            else {
                // overflow in line buffer, so discard and proceed:
                LOG("  lineBuf OVERFLOW %d(%d)", i-hanRead, LINE_BUF_SIZE-1);
                if (_overflowCallback) {
                    _overflowCallback(lineBuf, LINE_BUF_SIZE);
                }
                memset(lineBuf, 0, LINE_BUF_SIZE);
                hanRead = i+1;
            }
            i++;
        }
    }
}

uint16_t hanOnData(const char *data, uint16_t len, bool process) {
    for (uint16_t i = 0; i < len; i++) {
        hanBuf[(hanWritten+i)&HAN_BUF_MASK] = data[i];
    }
    hanWritten += len;
    if (HAN_BUF_SIZE < hanWritten-hanRead) {
        LOG("serialEvent1 OVERFLOW %d(%d)", hanRead, hanWritten);
        hanRead = hanWritten - HAN_BUF_SIZE;
    }
    if (process) {
        _hanProcess();
    }
    return len;
}

void _onReceiveError(hardwareSerial_error_t hse) {
    LOG("_onReceiveError %d", hse);
}

static const char* magicFormat = \
    "/LGF5E360\\ABC123\r\n\r\n" \
	"0-0:1.0.0(220907135912W)\r\n" \
	"1-0:1.8.0(%08d.%03d*kWh)\r\n" \
	"1-0:2.8.0(%08d.%03d*kWh)\r\n" \
	"1-0:1.7.0(%04d.%03d*kW)\r\n" \
	"1-0:2.7.0(%04d.%03d*kW)\r\n" \
	"1-0:21.7.0(%04d.%03d*kW)\r\n" \
	"1-0:42.7.0(%04d.%03d*kW)\r\n" \
	"1-0:32.7.0(0240.3*V)\r\n" \
	"1-0:31.7.0(%04d.%03d*A)\r\n" \
	"!";
void hanInjectMagic(uint32_t bootButtonDownMillis) {
    char magicMessage[HAN_BUF_SIZE];
    const uint32_t ms = millis();
    uint16_t crc = 0;
    sprintf(magicMessage, magicFormat,
        ms/1000000, (ms/1000)%1000, // Wh
        ms/10000000, (ms/10000)%1000, // Wh P
        bootButtonDownMillis/1000, bootButtonDownMillis%1000, // W
        (bootButtonDownMillis>>3)/1000, (bootButtonDownMillis>>3)%1000, // P
        (bootButtonDownMillis>>1)/1000, (bootButtonDownMillis>>1)%1000, // L1
        (bootButtonDownMillis>>4)/1000, (bootButtonDownMillis>>4)%1000, // P2
        (bootButtonDownMillis<<1)/1000, (bootButtonDownMillis<<1)%1000 // I1 in A, not kA
    );
//    LOG("writing %d chars on HAN buffer", strlen(magicMessage));
    hanOnData(magicMessage, strlen(magicMessage));
    crc = updateCRC(0, magicMessage, strlen(magicMessage));
    sprintf(magicMessage, "%04X\r\n", crc);
//    LOG("writing CRC %04X on HAN buffer", crc);
    hanOnData(magicMessage, strlen(magicMessage));
    _hanProcess();
}

void serialEvent1() {
    char buf[1024];
    while (Serial1.available()) {
        memset(buf, 0, 1024);
        uint16_t count = Serial1.read(buf, min(Serial1.available(), 1000));
        hanOnData(buf, count, true);
    }
}

void hanSetup(void (*lineCallback)(const char *line), void (*overflowCallback)(const char *line, size_t len))
{
    uint8_t hanChannel = prefs()->getUChar(PREFS_HW_HAN_CHANNEL, 0);
    bool disablePullup = prefs()->getBool(PREFS_PULLUP_DIS, false);

    _lineCallback = lineCallback;
    _overflowCallback = overflowCallback;

    memset(hanBuf, 0, HAN_BUF_SIZE);
    memset(hanPrefix, 0, 5);
    sprintf(hanPrefix, "-%d:", hanChannel); // date and time does not have 1-0: but 0-0:

    // On C3, HAN TX goes to GPIO6
    Serial1.setRxBufferSize(HAN_BUF_SIZE);
    Serial1.begin(115200, SERIAL_8N1, GPIO_HAN_DATA, -1, true); // ports only used on ESP32 for Serial1
    if (disablePullup) {
        gpio_pullup_dis(GPIO_NUM_6); // GPIO_HAN_DATA);
    }
    Serial1.onReceiveError(_onReceiveError);
    LOG("HAN setup GPIO %d, pullup %s", GPIO_HAN_DATA,
        disablePullup ? "disabled" : "internal");
    pinMode(GPIO_HAN_RTS, OUTPUT);
    digitalWrite(GPIO_HAN_RTS, HIGH);
}
