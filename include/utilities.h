#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace std;

enum FilterType : uint8_t {
    None,
    Sub,
    Up,
    Average,
    Paeth
};

enum ColorType : uint8_t {
    Grey,
    Invalid1,
    True,
    Indexed,
    GreyAlpha,
    Invalid2,
    TrueAlpha
};

enum BitDepth : uint8_t {
    One = 1,
    Two = 2,
    Four = 4,
    Eight = 8,
    Sixteen = 16
};

template<typename T>
T tobytes(float f);

template<>
uint8_t tobytes<uint8_t>(float f);
template<>
uint16_t tobytes<uint16_t>(float f);
template<>
uint32_t tobytes<uint32_t>(float f);

//print an unsigned integer into an ostream in msb format
void printInt(ostream &os, const uint8_t integer);
//print an unsigned integer into an ostream in msb format
void printInt(ostream &os, const uint16_t integer);
//print an unsigned integer into an ostream in msb format
void printInt(ostream &os, const uint32_t integer);


//push an unsigned integer into a vector of bytes in msb format
void pushInt(vector<uint8_t> &vec, const uint8_t integer);
//push an unsigned integer into a vector of bytes in msb format
void pushInt(vector<uint8_t> &vec, const uint16_t integer);
//push an unsigned integer into a vector of bytes in msb format
void pushInt(vector<uint8_t> &vec, const uint32_t integer);

#include "utilities.tpp"

#endif