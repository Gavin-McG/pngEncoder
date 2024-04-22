#ifndef BITSTREAM_TPP
#define BITSTREAM_TPP

#include "bitstream.h"

template<typename T>
void iBitstream::push(T integer) {
    size_t s = sizeof(integer);
    for (size_t i=1; i<=s; ++i) {
        pushBit((integer>>(s-i))%2);
    }
}

template<typename T>
void iBitstream::push(T integer, size_t bits) {
    for (size_t i=1; i<=bits; ++i) {
        pushBit((integer>>(bits-i))%2);
    }
}

template<typename T>
void iBitstream::pushReverse(T integer) {
    size_t s = sizeof(integer);
    for (size_t i=s; i>0; --i) {
        pushBit((integer>>(s-i))%2);
    }
}

template<typename T>
void iBitstream::pushReverse(T integer, size_t bits) {
    for (size_t i=bits; i>0; --i) {
        pushBit((integer>>(bits-i))%2);
    }
}








template<typename T>
T oBitstream::get(size_t bits) {
    T result = 0;
    for (size_t i=0;i<bits;++i) {
        result = (result<<1) + nextBit();
    }
    return result;
}

template<typename T>
T oBitstream::getReverse(size_t bits) {
    T result = 0;
    for (size_t i=0;i<bits;++i) {
        result += nextBit()<<i;
    }
    return result;
}


#endif