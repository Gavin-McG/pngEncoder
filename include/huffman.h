#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <algorithm>
#include <vector>
#include <bit>
#include <iostream>
#include "bitstream.h"
#include "utilities.h"
#include "lz77.h"

using namespace std;


//compression methods

//return the Flevel flag required to make the 16bit zlib flat a multiple of 31 as required
uint8_t FCheck(uint8_t cmf, uint8_t flg);

//pack literals into uncompressed stream with zlib info (least compressed)
vector<uint8_t> huffman_uncompressed(vector<uint8_t> literals, uint32_t adler);

//static coding helper functions
void literalStatic(iBitstream &bs, uint16_t literal);
void addLengthCode(iBitstream &bs, uint16_t length);
void lengthStatic(iBitstream &bs, uint16_t lengthCode);
void addDistanceCode(iBitstream &bs, uint16_t distance);
void distanceStatic(iBitstream &bs, uint16_t distanceCode);

//compress codes using static huffman coding (somewhat compressed)
vector<uint8_t> huffman_static(vector<Code> codes, uint32_t adler);

//compress codes using dynamic huffman coding (most compressed)
vector<uint8_t> huffman_dynamic(vector<Code> codes, uint32_t adler);





//decompression methods

//take a set of bytes and decompress using deflate methods
vector<Code> huffman_decompress(oBitstream &bs);

//decompress data of uncompressed format
void decompress_uncompressed(oBitstream &bs, vector<Code> &codes);

//decompress data of static format
void decompress_static(oBitstream &bs, vector<Code> &codes);

//decompress data of dynamic format
void decompress_dynamic(oBitstream &bs, vector<Code> &codes);








#include "bitstream.tpp"

#endif