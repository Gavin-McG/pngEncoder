#include "../include/utilities.h"

void printInt(ostream &os, const uint8_t integer) {
    os << integer;
}

void printInt(ostream &os, const uint16_t integer) {
    os << static_cast<uint8_t>(integer&uint8_t(255));
    os << static_cast<uint8_t>((integer<<8)&uint8_t(255));
}

void printInt(ostream &os, const uint32_t integer) {
    for (int i=24;i>=0;i-=8) {
        os << static_cast<uint8_t>((integer>>i)&uint8_t(255));
    }
}





void pushInt(vector<uint8_t> &vec, const uint8_t integer) {
    vec.push_back(integer);
}

void pushInt(vector<uint8_t> &vec, const uint16_t integer) {
    vec.push_back(static_cast<uint8_t>(integer&uint8_t(255)));
    vec.push_back(static_cast<uint8_t>(integer<<8));
}

void pushInt(vector<uint8_t> &vec, const uint32_t integer) {
    for (int i=24;i>=0;i-=8) {
        vec.push_back(static_cast<uint8_t>((integer>>i)&uint8_t(255)));
    }
}




//8 bit implimentation
template<>
uint8_t tobytes(float f) {
    f = clamp(f,0.0f,1.0f);
    return static_cast<uint8_t>(f*UINT8_MAX);
}

//16 bit implimentation
template<>
uint16_t tobytes<uint16_t>(float f) {
    f = clamp(f,0.0f,1.0f);
    return static_cast<uint16_t>(f*UINT16_MAX);
}

//32 bit implimentation
template<>
uint32_t tobytes<uint32_t>(float f) {
    f = clamp(f,0.0f,1.0f);
    return static_cast<uint32_t>(f*UINT32_MAX);
}




template<>
uint8_t readInt<uint8_t>(istream &is) {
    char c;
    is.get(c);
    return static_cast<uint8_t>(c);
}

template<>
uint16_t readInt<uint16_t>(istream &is) {
    uint8_t data[2];
    is.read(reinterpret_cast<char*>(data),2);
    uint16_t result = data[0];
    result = (result<<8)+data[1];
    return result;
}

template<>
uint32_t readInt<uint32_t>(istream &is) {
    uint8_t data[4];
    is.read(reinterpret_cast<char*>(data),4);
    uint32_t result = data[0];
    result = (result<<8)+data[1];
    result = (result<<8)+data[2];
    result = (result<<8)+data[3];
    return result;
}