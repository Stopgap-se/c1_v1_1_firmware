#ifndef _HAN_READER_H_
#define _HAN_READER_H_

uint16_t updateCRC(uint16_t crc, const char *data, size_t len);
void hanInjectMagic(uint32_t dMS);
uint16_t hanOnData(const char *data, uint16_t len, bool process = false);

void hanSetup(void (*lineCallback)(const char *line),
              void (*overflowCallback)(const char *line, size_t len));

#endif // _HAN_READER_H_