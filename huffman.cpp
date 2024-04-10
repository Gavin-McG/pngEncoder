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



uint16_t getLengthCode(uint16_t length) {
    //edge case 258
    if (length == 258) return 285;

    //shift length into range 0-255
    uint16_t shifted = length-3;

    //find the number of extra bits for the given length
    uint16_t log = bit_width(shifted);
    uint16_t xbits = log - min(static_cast<uint16_t>(3),log);

    //groups sets of 4 for the 4 codes of each extra bit size
    uint16_t r1 = shifted % (1<<xbits);
    uint16_t r2 = shifted % (1<<(xbits+2));
    uint16_t c = (r2-r1) >> xbits;

    //get code
    uint16_t code = c + 4*xbits + (shifted>=4?4:0) + 257;
    return code;
}

void addLengthXbits(Bitstream &bs, uint16_t length) {

}

vector<uint8_t> huffman_static(vector<Code> codes, uint32_t adler) {
    vector<uint8_t> vec;

    //flags
    uint8_t cmf = 0x78;
    uint8_t flg = 0x01;
    vec.push_back(cmf);
    vec.push_back(flg+FCheck(cmf,flg));

    //bitstream
    Bitstream bs;

    //header
    bs.push(static_cast<uint8_t>(0x06),3); //last block, static codes

    //push data to bitstream
    for (Code c : codes) {
        switch (c.type) {
            case CodeType::Literal:

                break;
            case CodeType::Length:
                
                break;
            case CodeType::Distance:
                
                break;
            default: break;
        }
    }

    //add data to byte vector
    vector<uint8_t> data = bs.bytesClear();
    for (uint i : data) {
        vec.push_back(i);
    }

    //adler
    pushInt(vec, adler);

    return vec;
}

vector<uint8_t> huffman_dynamic(vector<Code> codes, uint32_t adler) {
    return vector<uint8_t>();
}