#include "../include/crc32.h"


void generateCRC(uint32_t* crcTable) {
    for (int i=0;i<=255;++i) {
        uint32_t n = i;
        for (int j=0; j<8; ++j) {
            n = (n&1)==0 ? n>>1 : (n>>1)^CRC_POLY;
        }
        crcTable[i] = n;
    }
}

uint32_t getCRC32(stringstream &ss, const uint32_t* crcTable, bool flip, uint32_t initial) {
    string s = ss.str();
    uint32_t crc = initial;
    for (uint8_t c : s) {
        crc = (crc>>8) ^ crcTable[(crc^c) & 0xFF];
    }
    if (flip) crc ^= 0xFFFFFFFF;
    return crc;
}

uint32_t getCRC32(const string &s, const uint32_t* crcTable, bool flip, uint32_t initial) {
    uint32_t crc = initial;
    for (uint8_t c : s) {
        crc = (crc>>8) ^ crcTable[(crc^c) & 0xFF];
    }
    if (flip) crc ^= 0xFFFFFFFF;
    return crc;
}

uint32_t getCRC32(const char* s, size_t length, const uint32_t* crcTable, bool flip, uint32_t initial) {
    uint32_t crc = initial;
    for (size_t i=0; i<length; ++i) {
        uint8_t c = s[i];
        crc = (crc>>8) ^ crcTable[(crc^c) & 0xFF];
    }
    if (flip) crc ^= 0xFFFFFFFF;
    return crc;
}