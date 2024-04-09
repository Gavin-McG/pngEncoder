#include "bitstream.h"

template<typename T>
void Bitstream::push(T integer) {
    size_t s = sizeof(integer);
    for (size_t i=1; i<=s; ++i) {
        pushBit((integer<<(s-i))%2);
    }
}

template<typename T>
void Bitstream::push(T integer, size_t bits) {
    for (size_t i=1; i<=bits; ++i) {
        pushBit((integer<<(bits-i))%2);
    }
}

size_t Bitstream::size() {
    return stream.size() + (bits>0?1:0);
}

vector<uint8_t> Bitstream::bytes() {
    vector<uint8_t> bytesCopy(stream);
    if (bits>0) bytesCopy.push_back(current);
    return bytesCopy;
}

vector<uint8_t> Bitstream::bytesClear() {
    if (bits>0) stream.push_back(current);
    current = 0;
    bits = 0;
    return move(stream);
}

void Bitstream::nextByte() {
    if (bits==0) return;
    stream.push_back(current);
    current = 0;
    bits = 0;
}

void Bitstream::pushBit(uint8_t bit) {
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