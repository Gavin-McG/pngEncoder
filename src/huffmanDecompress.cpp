#include "../include/huffman.h"





vector<Code> huffman_decompress(oBitstream &bs) {
    vector<Code> codes;

    bool lastBlock = false;
    uint8_t deflateMethod = 0;
    while (!lastBlock) {
        //get header
        lastBlock = bs.getRL<uint8_t>(1)==1;
        deflateMethod = bs.getRL<uint8_t>(2);

        //retrieve codes from data
        if (deflateMethod == 0) {
            decompress_uncompressed(bs, codes);
        }else if (deflateMethod == 1) {
            decompress_static(bs, codes);
        }else if (deflateMethod == 2) {
            decompress_dynamic(bs, codes);
        }else{
            cerr << "Incorrect delfate method read" << endl;
        }
    }

    return codes;
}






void decompress_uncompressed(oBitstream &bs, vector<Code> &codes) {
    bs.nextByte();

    uint16_t length = bs.getRL<uint16_t>(16);
    uint16_t nlength = bs.getRL<uint16_t>(16);

    if ((length^nlength) != UINT16_MAX) {
        cerr << "LEN and NLEN do not match" << endl;
        cerr << "\tlength: " << length << endl;
        cerr << "\tnlength: " << nlength << endl;
        cerr << "\tXOR: " << (length^nlength) << endl;
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

    //check for invalid codes
    if (codeVal>=286) {
        cerr << "Invalid length code: " << codeVal << endl;
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
        cerr << "Invalid distance code: " << codeVal << endl;
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
}









uint16_t readDynamicCode(oBitstream &bs, const unordered_map<DynamicCode,uint16_t> &codes) {
    DynamicCode code;
    do {
        code.val = (code.val<<1)+bs.getLR<uint16_t>(1);
        code.length++;
    }while (codes.find(code)==codes.end());

    return codes.find(code)->second;
}

Code decodeLLCodeDynamic(oBitstream &bs, const uint16_t codeVal) {
    //check for invalid codes
    if (codeVal>=286) {
        cerr << "Invalid length code: " << codeVal << endl;
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

Code decodeDistCodeDynamic(oBitstream &bs, const uint16_t codeVal) {
    if (codeVal>=30) {
        cerr << "Invalid distance code: " << codeVal << endl;
    }

    uint8_t xBits = distXbits[codeVal];
    uint16_t rangeVal = bs.getRL<uint16_t>(xBits);
    return Code(CodeType::Distance,rangeVal + distValues[codeVal]);
}

void decompress_dynamic(oBitstream &bs, vector<Code> &codes) {
    //get counts
    uint8_t HLIT = bs.getRL<uint8_t>(5);
    uint8_t HDIST = bs.getRL<uint8_t>(5);
    uint8_t HCLEN = bs.getRL<uint8_t>(4);

    //read HCLEN+4 code lengths for length encoding alphabet
    vector<size_t> CLCLengths;
    for (uint8_t i=0; i<HCLEN+4; ++i) {
        CLCLengths.push_back(bs.getRL<size_t>(3));
    }
    //sort CLCCodes
    vector<size_t> CLCLengthsSorted(19);
    for (size_t i=0; i<CLCLengths.size();++i) {
        CLCLengthsSorted[CLCOrder[i]] = CLCLengths[i];
    }
    
    //get length alphabet codes from CLCLengths
    vector<DynamicCode> CLCodes = lengths2Codes(CLCLengthsSorted,7);
    
    //create hashMap for CLCodes
    unordered_map<DynamicCode, uint16_t> CLCodesHash;
    for (size_t i=0;i<CLCodes.size();++i) {
        CLCodesHash[CLCodes[i]] = i;
    }

    //read lit/dist code lengths using code length alphabet
    vector<size_t> fullLengths;
    while (fullLengths.size() < 258 + HLIT + HDIST) {
        uint16_t val = readDynamicCode(bs, CLCodesHash);
        if (val==18) {
            uint8_t extra = bs.getRL<uint8_t>(7);
            for (uint8_t i=0; i<11+extra; ++i) {
                fullLengths.push_back(0);
            }
        }else if (val==17) {
            uint8_t extra = bs.getRL<uint8_t>(3);
            for (uint8_t i=0; i<3+extra; ++i) {
                fullLengths.push_back(0);
            }
        }else if (val==16) {
            uint8_t extra = bs.getRL<uint8_t>(2);
            size_t ind = fullLengths.size()-1;
            for (uint8_t i=0; i<3+extra; ++i) {
                fullLengths.push_back(fullLengths[ind]);
            }
        }else{
            fullLengths.push_back(val);
        }
    }

    //seperate literal and distance codes
    vector<size_t> litLengths(fullLengths.begin(),fullLengths.begin()+257+HLIT);
    vector<size_t> distLengths(fullLengths.begin()+257+HLIT,fullLengths.end());

    //get codes from lengths
    vector<DynamicCode> litCodes = lengths2Codes(litLengths,15);
    vector<DynamicCode> distCodes = lengths2Codes(distLengths,15);
    //create hashtable of codes
    unordered_map<DynamicCode, uint16_t> litCodesHash;
    unordered_map<DynamicCode, uint16_t> distCodesHash;
    for (size_t i=0;i<litCodes.size();++i) {
        litCodesHash[litCodes[i]] = i;
    }
    for (size_t i=0;i<distCodes.size();++i) {
        distCodesHash[distCodes[i]] = i;
    }
    
    while (true) {
        uint16_t readCode = readDynamicCode(bs,litCodesHash);

        //get new lz77 code
        Code newCode = decodeLLCodeDynamic(bs,readCode);

        //check for end of block
        if (newCode.val==256) {
            break;
        }

        if (newCode.type==CodeType::Length) {
            codes.push_back(newCode);
            uint16_t readDist = readDynamicCode(bs, distCodesHash);
            codes.push_back(decodeDistCodeDynamic(bs,readDist));
        }else{
            codes.push_back(newCode);
        }
    }
}