#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>

using namespace std;

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


template<typename T>
T readInt(istream &is);

template<>
uint8_t readInt<uint8_t>(istream &is);
template<>
uint16_t readInt<uint16_t>(istream &is);
template<>
uint32_t readInt<uint32_t>(istream &is);


template<typename T>
T packInt(char* data);


#include "utilities.tpp"

#endif