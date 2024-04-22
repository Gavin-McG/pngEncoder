#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <vector>

using namespace std;

class  iBitstream {
    public:

    iBitstream() : current(0), bits(0) {}

    template<typename T>
    void push(T integer);

    template<typename T>
    void push(T integer, size_t bits);

    template<typename T>
    void pushReverse(T integer);

    template<typename T>
    void pushReverse(T integer, size_t bits);

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



class oBitstream {
    public:

    oBitstream(char* data) : index(0), bits(0), data(data) {}

    template<typename T>
    T get(size_t bits);

    template<typename T>
    T getReverse(size_t bits);

    uint8_t nextBit();
    void skipByte();

    private:
    uint8_t index;
    uint8_t bits;
    char* data;
};



#endif