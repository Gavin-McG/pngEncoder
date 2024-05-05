#ifndef LZ77_H
#define LZ77_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <unordered_map>

using namespace std;

static const size_t windowSize = 32768;
static const size_t lengthSize = 258;

enum CodeType {
    Literal,
    Length,
    Distance
};

struct Code {
    CodeType type;
    uint16_t val;

    Code (CodeType type, uint16_t val) : type(type), val(val) {}
};



void printLZ77Codes(const vector<Code> &codes);


vector<Code> lz77_compress(vector<uint8_t> &literals, bool debug = false);

void lz77_decompress(vector<Code> &codes, vector<uint8_t> &literals, bool debug = false);

#endif