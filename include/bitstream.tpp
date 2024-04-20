#ifndef BITSTREAM_TPP
#define BITSTREAM_TPP

#include "bitstream.h"

template<typename T>
void Bitstream::push(T integer) {
    size_t s = sizeof(integer);
    for (size_t i=1; i<=s; ++i) {
        pushBit((integer>>(s-i))%2);
    }
}

template<typename T>
void Bitstream::push(T integer, size_t bits) {
    for (size_t i=1; i<=bits; ++i) {
        pushBit((integer>>(bits-i))%2);
    }
}

template<typename T>
void Bitstream::pushReverse(T integer) {
    size_t s = sizeof(integer);
    for (size_t i=s; i>0; --i) {
        pushBit((integer>>(s-i))%2);
    }
}

template<typename T>
void Bitstream::pushReverse(T integer, size_t bits) {
    for (size_t i=bits; i>0; --i) {
        pushBit((integer>>(bits-i))%2);
    }
}

#endif