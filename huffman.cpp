#include "huffman.h"



uint8_t FCheck(uint8_t cmf, uint8_t flg) {
    uint16_t combined = static_cast<uint16_t>(cmf)*256 + flg;
    uint8_t remainder = combined%31;
    return (31 - remainder);
}

vector<uint8_t> huffman_uncompressed(vector<uint8_t> literals, uint32_t adler) {
    vector<uint8_t> vec;

    //flags
    uint8_t cmf = 0x08;
    uint8_t flg = 0x00;
    vec.push_back(cmf);
    vec.push_back(flg+FCheck(cmf,flg));

    //block header
    vec.push_back(0x01);

    //lengths
    vec.push_back(static_cast<uint8_t>((literals.size()) & UINT8_MAX));
    vec.push_back(static_cast<uint8_t>((literals.size()>>8) & UINT8_MAX));
    vec.push_back(static_cast<uint8_t>((literals.size()) & UINT8_MAX) ^ UINT8_MAX);
    vec.push_back(static_cast<uint8_t>((literals.size()>>8) & UINT8_MAX) ^ UINT8_MAX);

    //data
    for (uint8_t i : literals) {
        vec.push_back(i);
    }

    //adler
    pushInt(vec, adler);

    return vec;
}

vector<uint8_t> huffman_static(vector<uint16_t> codes, uint32_t adler) {
    return vector<uint8_t>();
}

vector<uint8_t> huffman_dynamic(vector<uint16_t> codes, uint32_t adler) {
    return vector<uint8_t>();
}