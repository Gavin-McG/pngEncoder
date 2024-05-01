#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <algorithm>
#include <vector>
#include <bit>
#include <iostream>
#include <iomanip>
#include "bitstream.h"
#include "utilities.h"
#include "lz77.h"
#include "dynamicTree.h"

using namespace std;


//compression methods

//return the Flevel flag required to make the 16bit zlib flat a multiple of 31 as required
uint8_t FCheck(uint8_t cmf, uint8_t flg);

//pack literals into uncompressed stream with zlib info (least compressed)
vector<uint8_t> huffman_uncompressed(vector<uint8_t> literals, uint32_t adler);



//helper functions for static & dynamic coding
uint16_t getLengthXbits(uint16_t length);
uint16_t getLengthCode(uint16_t length);
void addLengthXbits(iBitstream &bs, uint16_t length);

uint16_t getDistanceCode(uint16_t distance);
uint16_t getDistanceXbits(uint16_t distance);
void addDistanceXbits(iBitstream &bs, uint16_t distance);



//static coding helper functions
void literalStatic(iBitstream &bs, uint16_t literal);

void addLengthCode(iBitstream &bs, uint16_t length);
void lengthStatic(iBitstream &bs, uint16_t lengthCode);

void addDistanceCode(iBitstream &bs, uint16_t distance);
void distanceStatic(iBitstream &bs, uint16_t distanceCode);

//compress codes using static huffman coding (somewhat compressed)
vector<uint8_t> huffman_static(vector<Code> codes, uint32_t adler);



//dynamic coding helper functions
const vector<uint8_t> CLCOrder = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

vector<pair<uint8_t,uint8_t>> codeLengthEncoding(vector<DynamicCode> &codes);

void addLiteralDynamic(iBitstream &bs, uint16_t code, vector<DynamicCode> &codes);

void addLengthCode(iBitstream &bs, uint16_t length, vector<DynamicCode> &codes);

void addDistanceCode(iBitstream &bs, uint16_t distance, vector<DynamicCode> &codes);

//compress codes using dynamic huffman coding (most compressed)
vector<uint8_t> huffman_dynamic(vector<Code> codes, uint32_t adler);





//decompression methods

//take a set of bytes and decompress using deflate methods
vector<Code> huffman_decompress(oBitstream &bs);

//decompress data of uncompressed format
void decompress_uncompressed(oBitstream &bs, vector<Code> &codes);

const vector<uint16_t> lengthValues = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,257,258};
const vector<uint8_t> lengthXbits = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};

const vector<uint16_t> distValues = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
const vector<uint8_t> distXbits = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

//decompress data of static format
void decompress_static(oBitstream &bs, vector<Code> &codes);

//decompress data of dynamic format
void decompress_dynamic(oBitstream &bs, vector<Code> &codes);








#include "bitstream.tpp"

#endif