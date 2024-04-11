#ifndef DEFLATE_H
#define DEFLATE_H

#include <vector>
#include <stdint.h>
#include "huffman.h"
#include "adler32.h"
#include "lz77.h"

using namespace std;


enum DeflateType {
    NoCompression,
    StaticCodes,
    DynamicCodes
};


vector<uint8_t> deflateNoCompression(vector<uint8_t> &data);
vector<uint8_t> deflateStatic(vector<uint8_t> &data);
vector<uint8_t> deflateDynamic(vector<uint8_t> &data);


vector<uint8_t> deflate(vector<uint8_t> &data, DeflateType deflateMethod);

#endif