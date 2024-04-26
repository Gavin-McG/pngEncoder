#include "../include/bitstream.h"


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









size_t oBitstream::size() {
    return length;
}

bool oBitstream::empty() {
    return index>=length;
}

uint8_t oBitstream::nextBit() {
    if (empty()) return 0;
    uint8_t result = (static_cast<uint8_t>(data[index])>>bits)%2;
    //cout << "index: " << static_cast<int>(index) << "  bit: " << static_cast<int>(bits) << "  val: " << static_cast<int>(result) << endl;
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
