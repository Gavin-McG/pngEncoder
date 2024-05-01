#include "../include/huffman.h"





vector<Code> huffman_decompress(oBitstream &bs) {
    vector<Code> codes;

    bool lastBlock = false;
    uint8_t deflateMethod = 0;
    while (!lastBlock) {
        //get header
        lastBlock = bs.getRL<uint8_t>(1)==1;
        deflateMethod = bs.getRL<uint8_t>(2);

        cout << "read deflate method " << static_cast<int>(deflateMethod) << endl;

        //retrieve codes from data
        if (deflateMethod == 0) {
            decompress_uncompressed(bs, codes);
        }else if (deflateMethod == 1) {
            decompress_static(bs, codes);
        }else if (deflateMethod == 2) {
            decompress_dynamic(bs, codes);
        }else{
            cout << "Incorrect delfate method read" << endl;
        }
    }

    return codes;
}






void decompress_uncompressed(oBitstream &bs, vector<Code> &codes) {
    bs.nextByte();

    uint16_t length = bs.getRL<uint16_t>(16);
    uint16_t nlength = bs.getRL<uint16_t>(16);

    if ((length^nlength) != UINT16_MAX) {
        cout << "LEN and NLEN do not match" << endl;
        cout << "\tlength: " << length << endl;
        cout << "\tnlength: " << nlength << endl;
        cout << "\tXOR: " << (length^nlength) << endl;
    }

    for (size_t i=0;i<length; ++i) {
        codes.emplace_back(CodeType::Literal,bs.getRL<uint16_t>(8));
    }
}







bool checkStaticCode(const DynamicCode &code) {
    //check whether a code is a valid code for static encoding
    if (code.length==7 && code.val>=0 && code.val<=23) {
        return true;
    }else if (code.length==8 && code.val>=48 && code.val<=199) {
        return true;
    }else if (code.length==9 && code.val>=400 && code.val<=511) {
        return true;
    }
    return false;
}

Code decodeLLCodeStatic(oBitstream &bs, const DynamicCode &code) {
    uint16_t codeVal = 0;
    //get the 0-285 value of code
    if (code.length==7 && code.val>=0 && code.val<=23) {
        codeVal = 256 + code.val - 0;
    }else if (code.length==8 && code.val>=48 && code.val<=191) {
        codeVal = 0 + code.val - 48;
    }else if (code.length==8 && code.val>=192 && code.val<=199) {
        codeVal = 280 + code.val - 192;
    }else if (code.length==9 && code.val>=400 && code.val<=511) {
        codeVal = 144 + code.val - 400;
    }

    //cout << '\t' << setw(3) << codeVal << " | " << static_cast<int>(code.length) << ' ' << code.val << endl;

    //check for invalid codes
    if (codeVal>=286) {
        cout << "Invalid length code: " << codeVal << endl;
    }

    //literals/end of block code
    if (codeVal<=256) {
        //return literal/end of block code
        return Code(CodeType::Literal,codeVal);
    }

    //return length code
    uint8_t xBits = lengthXbits[codeVal-257];
    uint16_t rangeVal = bs.getRL<uint16_t>(xBits);
    return Code(CodeType::Length,rangeVal + lengthValues[codeVal-257]);
}

Code decodeDistCodeStatic(oBitstream &bs) {
    uint16_t codeVal = bs.getLR<uint16_t>(5);

    if (codeVal>=30) {
        cout << "Invalid distance code: " << codeVal << endl;
    }

    uint8_t xBits = distXbits[codeVal];
    uint16_t rangeVal = bs.getRL<uint16_t>(xBits);
    return Code(CodeType::Distance,rangeVal + distValues[codeVal]);
}

void decompress_static(oBitstream &bs, vector<Code> &codes) {
    while (true) {
        DynamicCode readCode = DynamicCode(7,bs.getLR<uint16_t>(7));
        //advance to 8-bit code
        if (!checkStaticCode(readCode)) {
            readCode.val = (readCode.val<<1)+bs.getLR<uint16_t>(1);
            readCode.length++;
            //advance to 9-bit code
            if (!checkStaticCode(readCode)) {
                readCode.val = (readCode.val<<1)+bs.getLR<uint16_t>(1);
                readCode.length++;
            }
        }

        //get new lz77 code
        Code newCode = decodeLLCodeStatic(bs,readCode);

        //check for end of block
        if (newCode.val==256) {
            break;
        }

        if (newCode.type==CodeType::Length) {
            codes.push_back(newCode);
            codes.push_back(decodeDistCodeStatic(bs));
        }else{
            codes.push_back(newCode);
        }
    }

    // for (Code c : codes) {
    //     if (c.type==Literal) {
    //         cout << "L";
    //     }else if (c.type==Length) {
    //         cout << "P";
    //     }else if (c.type==Distance) {
    //         cout << "D";
    //     }
    // }
    cout << endl;
}









void decompress_dynamic(oBitstream &bs, vector<Code> &codes) {
}