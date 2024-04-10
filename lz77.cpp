#include "lz77.h"



vector<Code> lz77_compress(vector<uint8_t> &literals) {
    vector<Code> encoded;

    //loop through until all bytes have been encoded
    uint16_t i=0;
    while(i<literals.size()) {
        uint16_t maxLength = 0;
        uint16_t distance = 0;

        //check full window as potential starting point for pointer
        for (uint16_t j = i<windowSize?0:i-windowSize; j<i; ++j) {
            uint16_t current = 0;

            //check until max length or non-match found
            for (uint16_t k=0; k<lengthSize && i+k<literals.size(); ++k) {
                if (literals[i+k]!=literals[j+k]) break;
                ++current;
            }

            //update best option
            if (current >= maxLength) {
                maxLength = current;
                distance = i-j;
            }
        }
        
        if (maxLength>=3) {
            //add pointer to codes
            encoded.push_back(Code(CodeType::Length,maxLength));
            encoded.push_back(Code(CodeType::Distance,distance));
            i+=maxLength;
        }else{
            //add literal to codes
            encoded.push_back(Code(CodeType::Literal,literals[i]));
            ++i;
        }
    }

    return encoded;
}