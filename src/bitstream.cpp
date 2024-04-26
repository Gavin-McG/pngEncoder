#include "../include/bitstream.h"


void iBitstream::nextByte(bool force) {
    if (bits>0 || force) {
        stream.push_back(current);
        bits = 0;
        current = 0;
    }
}

size_t iBitstream::sizeBytes() {
    return stream.size() + (bits>0?1:0);
}
size_t iBitstream::sizeBits() {
    return stream.size()*8 + bits;
}

bool iBitstream::empty() {
    return stream.size()==0 && bits==0;
}

vector<uint8_t> iBitstream::getBytes() {
    vector<uint8_t> streamCopy(stream.begin(),stream.end());
    if (bits>0) streamCopy.push_back(current);
    return streamCopy;
}
vector<uint8_t> iBitstream::getBytesMove() {
    if (bits>0) stream.push_back(current);
    current = 0;
    bits = 0;
    return move(stream);
}

void iBitstream::clear() {
    stream = vector<uint8_t>();
    bits = 0;
    current = 0;
}


void iBitstream::pushBit(uint8_t bit) {
    if (order==BitOrder::LSBitFirst) {
        current += (bit<<bits);
    }else{
        current += (bit<<(7-bits));
    }

    ++bits;
    if (bits==8) {
        stream.push_back(current);
        bits=0;
        current=0;
    }
}









void oBitstream::nextByte(bool force) {
    if (bits>0 || force) {
        ++index;
        bits = 0;
    }
}

bool oBitstream::empty() {
    return index==stream.size();
}

void oBitstream::clear() {
    stream = vector<uint8_t>();
    index = 0;
    bits = 0;
}

void oBitstream::setOrder(BitOrder newOrder) {
    nextByte(false);
    order = newOrder;
}

uint8_t oBitstream::getBit() {
    if (empty()) return 0;

    uint8_t result = (stream[index] >> (order==BitOrder::LSBitFirst ? bits : 7-bits))%2;

    ++bits;
    if (bits==8) {
        ++index;
        bits=0;
    }

    return result;
}
