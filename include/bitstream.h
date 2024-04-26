#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <vector>
#include <iostream>

using namespace std;

enum BitOrder {
    MSBitFirst,
    LSBitFirst
};

class iBitstream {
    public:

    iBitstream(BitOrder order = BitOrder::LSBitFirst) : current(0), bits(0), order(order) {}

    template<typename T>
    void pushRL(T integer);
    template<typename T>
    void pushRL(T integer, size_t bits);

    template<typename T>
    void pushLR(T integer);
    template<typename T>
    void pushLR(T integer, size_t bits);

    size_t sizeBytes();
    size_t sizeBits();

    bool empty();

    vector<uint8_t> getBytes();
    vector<uint8_t> getBytesMove();

    void clear();

    private:
    uint8_t current;
    uint8_t bits;
    BitOrder order;
    vector<uint8_t> stream;

    //takes either a 0 or 1 as input and pushes it into the stream
    void pushBit(uint8_t bit);
};





class oBitstream {
    public:

    oBitstream(char* data, size_t length) : index(0), bits(0), data(data), length(length) {}

    size_t size();

    bool empty();

    template<typename T>
    T get(size_t bits);

    template<typename T>
    T getReverse(size_t bits);

    uint8_t nextBit();
    void skipByte();

    private:
    size_t index;
    uint8_t bits;
    char* data;
    size_t length;
};



#endif