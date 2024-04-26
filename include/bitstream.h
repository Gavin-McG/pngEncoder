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

    void nextByte(bool force = false);

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

    oBitstream(vector<uint8_t> data, BitOrder order = BitOrder::LSBitFirst) : index(0), bits(0), order(order), stream(data) {}

    template<typename T>
    T getRL();
    template<typename T>
    T getRL(size_t bits);

    template<typename T>
    T getLR();
    template<typename T>
    T getLR(size_t bits);

    void nextByte(bool force = false);

    bool empty();
    void clear();

    void setOrder(BitOrder newOrder);

    private:
    size_t index;
    uint8_t bits;
    BitOrder order;
    vector<uint8_t> stream;

    uint8_t getBit();
};



#endif