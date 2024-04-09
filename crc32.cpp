#include "crc32.h"

//polynomial used for CRC table hash
const uint32_t crcPoly = 0xEDB88320;

void generateCRC(uint32_t* crcTable) {
    for (int i=0;i<255;++i) {
        uint32_t n = i;
        for (int j=0; j<8; ++j) {
            n = (n&1)==0 ? n>>1 : (n>>1)^crcPoly;
        }
        crcTable[i] = n;
    }
}

uint32_t getCRC32(stringstream &ss, const uint32_t* crcTable) {
    string s = ss.str();
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t c : s) {
        crc = (crc>>8) ^ crcTable[(crc^c) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

uint32_t getCRC32(const string &s, const uint32_t* crcTable) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t c : s) {
        crc = (crc>>8) ^ crcTable[(crc^c) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}