#include "adler32.h"

uint32_t getADLER32(stringstream &ss) {
    string s = ss.str();

    uint16_t A=1, B=0;
    for (char c : s) {
        A += c;
        B += A;
    }
    return (static_cast<uint32_t>(B)<<16) + A;
}

uint32_t getADLER32(string s) {
    uint16_t A=1, B=0;
    for (char c : s) {
        A += c;
        B += A;
    }
    return (static_cast<uint32_t>(B)<<16) + A;
}

uint32_t getADLER32(vector<uint8_t> &vec) {
    uint16_t A=1, B=0;
    for (uint8_t c : vec) {
        A += c;
        B += A;
    }
    return (static_cast<uint32_t>(B)<<16) + A;
}