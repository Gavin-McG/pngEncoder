#include "adler32.h"

uint32_t getADLER32(stringstream &ss) {
    string s = ss.str();

    uint32_t A=1, B=0;
    for (char c : s) {
        A = (A+c) % MOD_ADLER;
        B = (B+A) % MOD_ADLER;
    }
    return (B<<16) + A;
}

uint32_t getADLER32(string s) {
    uint32_t A=1, B=0;
    for (char c : s) {
        A = (A+c) % MOD_ADLER;
        B = (B+A) % MOD_ADLER;
    }
    return (B<<16) + A;
}

uint32_t getADLER32(vector<uint8_t> &vec) {
    uint32_t A=1, B=0;
    for (uint8_t c : vec) {
        A = (A+c) % MOD_ADLER;
        B = (B+A) % MOD_ADLER;
    }
    return (B<<16) + A;
}