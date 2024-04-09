#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <vector>

using namespace std;

class  Bitstream {
    public:

    template<typename T>
    void push(T integer);

    template<typename T>
    void push(T integer, size_t bits);

    size_t size();

    vector<uint8_t> bytes();
    vector<uint8_t> bytesClear();

    void nextByte();

    private:
    uint8_t current;
    uint8_t bits;
    vector<uint8_t> stream;

    //takes either a 0 or 1 as input and pushes it into the stream
    void pushBit(uint8_t bit);
};

#endif