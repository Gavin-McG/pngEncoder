#include "../include/bitstream.h"


size_t iBitstream::size() {
    return stream.size() + (bits>0?1:0);
}

vector<uint8_t> iBitstream::bytes() {
    vector<uint8_t> bytesCopy(stream);
    if (bits>0) bytesCopy.push_back(current);
    return bytesCopy;
}

vector<uint8_t> iBitstream::bytesClear() {
    if (bits>0) stream.push_back(current);
    current = 0;
    bits = 0;
    return move(stream);
}

void iBitstream::nextByte() {
    if (bits==0) return;
    stream.push_back(current);
    current = 0;
    bits = 0;
}

void iBitstream::pushBit(uint8_t bit) {
    bit = bit%2; //input protection
    current += (bit<<bits);
    ++bits;

    //add current to vector
    if (bits==8) {
        stream.push_back(current);
        current = 0;
        bits = 0;
    }
}






uint8_t oBitstream::nextBit() {
    uint8_t result = (data[index]<<bits)%2;
    ++bits;

    //add current to vector
    if (bits==8) {
        ++index;
        bits = 0;
    }

    return result;
}

void oBitstream::skipByte() {
    ++index;
    bits = 0;
}
