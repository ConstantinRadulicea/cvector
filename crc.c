#include "crc.h"


uint16_t crc16_ccitt_init(void) {
	return (uint16_t)0xFFFF; // Initial value for CRC-16-CCITT
}

uint16_t crc16_ccitt(uint16_t crc, uint8_t a)
{
    crc ^= (uint16_t)a << (uint8_t)8;
    for (uint8_t ii = (uint8_t)0; ii < (uint8_t)8; ++ii) {
        crc = crc << (uint8_t)1;
        if (crc & (uint16_t)0x8000) {
            crc = crc ^ (uint16_t)0x1021;
        }
    }
    return crc;
}

uint16_t crc16_ccitt_update(uint16_t crc, const uint8_t* data, size_t len) {
    for (size_t i = (size_t)0; i < len; i++) {
        crc = crc16_ccitt(crc, data[i]);
    }
    return crc;
}

