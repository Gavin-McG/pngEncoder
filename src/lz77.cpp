#include "../include/lz77.h"




void printLZ77Codes(const vector<Code> &codes) {
    cout << codes.size() << " codes:" << endl;
    for (size_t i=0;i<codes.size();++i) {
        if (codes[i].type==CodeType::Literal) {
            cout << "L-";
        }else if (codes[i].type==CodeType::Length) {
            cout << "P-";
        }else{
            cout << "D-";
        }
        cout << left << setw(8) << setfill(' ') << codes[i].val;
        if (i%8==7) cout << '\n';
    }
    if (codes.size()%8!=0) {
        cout << '\n';
    }
}




vector<Code> lz77_compress(vector<uint8_t> &literals, bool debug) {
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
        }else{
            //add literal to codes
            encoded.push_back(Code(CodeType::Literal,literals[i]));
            ++i;
        }
    }

    //debug output
    if (debug) {
        printLZ77Codes(encoded);
    }

    return encoded;
}



void lz77_decompress(vector<Code> &codes, vector<uint8_t> &literals, bool debug) {
    //debug output
    if (debug) {
        printLZ77Codes(codes);
    }

    for (size_t i=0;i<codes.size();++i) {
        if (codes[i].type == CodeType::Literal) {
            literals.push_back(codes[i].val);
        }else if (codes[i].type == CodeType::Length) {
            if (i+1 == codes.size() || codes[i+1].type != CodeType::Distance) cerr << "lz77 length code missing distance code" << endl;
            for (size_t j=literals.size()-codes[i+1].val, k=0; k<codes[i].val; ++k, ++j) {
                literals.push_back(literals[j]);
            }
            ++i;
        }else{
            cerr << "Invalid lz77 code order - skipping distance code" << endl;
        }
    }
}