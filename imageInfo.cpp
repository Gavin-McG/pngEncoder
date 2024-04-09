#include "imageInfo.h"

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    int16_t p = static_cast<int16_t>(a) + static_cast<int16_t>(b) - static_cast<int16_t>(c);
    int16_t pa = abs(p - static_cast<int16_t>(a));
    int16_t pb = abs(p - static_cast<int16_t>(b));
    int16_t pc = abs(p - static_cast<int16_t>(c));
    if (pa <= pb && pa <= pc) return static_cast<uint8_t>(a);
    else if (pb <= pc) return static_cast<uint8_t>(b);
    return static_cast<uint8_t>(c);
}