#ifndef LZ77_H
#define LZ77_H

#include <stdint.h>
#include <vector>

using namespace std;

static const uint16_t windowSize = (UINT16_MAX>>1)+1;
static const uint16_t lengthSize = 258;

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

vector<Code> lz77_compress(vector<uint8_t> &literals);

#endif