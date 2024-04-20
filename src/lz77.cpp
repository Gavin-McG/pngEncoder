#include "../include/lz77.h"



vector<Code> lz77_compress(vector<uint8_t> &literals) {
    vector<Code> encoded;

    //loop through until all bytes have been encoded
    size_t i=0;
    while(i<literals.size()) {
        uint16_t maxLength = 0;
        uint16_t distance = 0;

        //check full window as potential starting point for pointer
        size_t lowerBound = i-min(windowSize,i);
        for (size_t j = i; j>lowerBound && maxLength<lengthSize; --j) {
            uint16_t k = 0;

            //check until max length or non-match found
            for (; k<lengthSize && i+k<literals.size(); ++k) {
                if (literals[i+k]!=literals[j-1+k]) break;
            }

            //update best option
            if (k > maxLength) {
                maxLength = k;
                distance = i-j+1;
            }
        }
        
        if (maxLength>=3) {
            //add pointer to codes
            encoded.push_back(Code(CodeType::Length,maxLength));
            encoded.push_back(Code(CodeType::Distance,distance));
            i+=maxLength;
            //cout << maxLength << ' ' << distance << endl;
        }else{
            //add literal to codes
            encoded.push_back(Code(CodeType::Literal,literals[i]));
            //cout << 1 << ' ' << literals[i] << endl;
            ++i;
        }
    }

    return encoded;
}