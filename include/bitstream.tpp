#ifndef BITSTREAM_TPP
#define BITSTREAM_TPP

#include "bitstream.h"

template<typename T>
void iBitstream::pushRL(T integer) {
    pushRL(integer, sizeof(T)*8);
}

template<typename T>
void iBitstream::pushRL(T integer, size_t bits) {
    for (size_t i=0; i<bits; ++i) {
        pushBit((integer>>i)%2);
    }
}


template<typename T>
void iBitstream::pushLR(T integer) {
    pushLR(integer, sizeof(T)*8);
}

template<typename T>
void iBitstream::pushLR(T integer, size_t bits) {
    //if (integer>255) cout << "PUSHED BIT LENGTH LARGER THAN 8" << endl;
    for (size_t i=1; i<=bits; ++i) {
        pushBit((integer>>(bits-i))%2);
    }
}








template<typename T>
T oBitstream::getRL() {
    return getRL<T>(sizeof(T)*8);
}

template<typename T>
T oBitstream::getRL(size_t bits) {
    T result = 0;
    for (size_t i=0; i<bits; ++i) {
        result += (getBit()<<i);
    }
    return result;
}


template<typename T>
T oBitstream::getLR() {
    return getLR<T>(sizeof(T)*8);
}

template<typename T>
T oBitstream::getLR(size_t bits) {
    T result = 0;
    for (size_t i=1; i<=bits; ++i) {
        result += (getBit()<<(bits-i));
    }
    return result;
}


#endif