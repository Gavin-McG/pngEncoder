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

    //block of max size 65536 bytes
    for (size_t i=0; i<literals.size(); i+=UINT16_MAX) {
        //block header
        if (literals.size()-i <= UINT16_MAX) {
            vec.push_back(0x01);
        }else{
            vec.push_back(0x00);
        }
        

        //lengths
        uint16_t length = static_cast<uint16_t>(min(static_cast<size_t>(UINT16_MAX),literals.size()-i));
        uint16_t nlength = length ^ UINT16_MAX;
        vec.push_back(length & UINT8_MAX);
        vec.push_back((length >> 8) & UINT8_MAX);
        vec.push_back(nlength & UINT8_MAX);
        vec.push_back((nlength >> 8) & UINT8_MAX);

        //data
        for (size_t j=i; j<literals.size() && j<i+UINT16_MAX; ++j) {
            vec.push_back(literals[j]);
        }
    }

    //adler
    pushInt(vec, adler);

    return vec;
}

vector<uint8_t> huffman_static(vector<Code> codes, uint32_t adler) {
    return vector<uint8_t>();
}

vector<uint8_t> huffman_dynamic(vector<Code> codes, uint32_t adler) {
    return vector<uint8_t>();
}