#include "../include/deflate.h"


//push data into uncompressed deflate format. clears data for memory saving
vector<uint8_t> deflateNoCompression(vector<uint8_t> &data) {
    //compute adler
    uint32_t adler = getADLER32(data);

    //convert data to huffman format
    vector<uint8_t> compressed = huffman_uncompressed(data, adler);
    data = vector<uint8_t>();
    
    return compressed;
}

//push data into static deflate format. clears data for memory saving
vector<uint8_t> deflateStatic(vector<uint8_t> &data) {
    //compute adler
    uint32_t adler = getADLER32(data);

    //run lz77 algorithm
    vector<Code> codes = lz77_compress(data);
    data = vector<uint8_t>();

    //push literals an lz77 pointers into static huffman coding
    vector<uint8_t> compressed = huffman_static(codes, adler);
    codes = vector<Code>();

    return compressed;
}

vector<uint8_t> deflateDynamic(vector<uint8_t> &data) {
    return vector<uint8_t>();
}


vector<uint8_t> deflate(vector<uint8_t> &data, DeflateType deflateMethod) {
    switch (deflateMethod) {
        case DeflateType::NoCompression:
            return deflateNoCompression(data);
        case DeflateType::StaticCodes:
            return deflateStatic(data);
        case DeflateType::DynamicCodes:
            return deflateDynamic(data);
        default:
            break;
    }
    return vector<uint8_t>();
}

