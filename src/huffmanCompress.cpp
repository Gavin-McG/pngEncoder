#include "../include/huffman.h"



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















uint16_t getLengthXbits(uint16_t length) {
    //shift length into range 0-255
    uint16_t shifted = length-3;

    //find the number of extra bits for the given length
    uint16_t log = bit_width(shifted);
    return log - min(static_cast<uint16_t>(3),log);
}

uint16_t getLengthCode(uint16_t length) {
    if (length==258) return 285;

    //shift length into range 0-255
    uint16_t shifted = length-3;

    uint16_t xbits = getLengthXbits(length);

    //groups sets of 4 for the 4 codes of each extra bit size
    uint16_t r1 = shifted % (1<<xbits);
    uint16_t r2 = shifted % (1<<(xbits+2));
    uint16_t c = (r2-r1) >> xbits;

    //get code
    return c + 4*xbits + (shifted>=4?4:0) + 257;
}

void addLengthXbits(iBitstream &bs, uint16_t length) {
    //shift length into range 0-255
    uint16_t shifted = length-3;

    uint16_t xbits = getLengthXbits(length);

    //get number to add as extra bits
    uint16_t r1 = shifted % (1<<xbits);
    
    //get code
    bs.pushRL(r1,xbits);
}



uint16_t getDistanceXbits(uint16_t distance) {
    //shift length into range
    uint16_t shifted = distance-1;

    //find the number of extra bits for the given length
    uint16_t log = bit_width(shifted);
    return log - min(static_cast<uint16_t>(2),log);
}

uint16_t getDistanceCode(uint16_t distance) {
    //shift length into range
    uint16_t shifted = distance-1;

    uint16_t xbits = getDistanceXbits(distance);

    //groups sets of 2 for the 2 codes of each extra bit size
    uint16_t r1 = shifted % (1<<xbits);
    uint16_t r2 = shifted % (1<<(xbits+1));
    uint16_t c = (r2-r1) >> xbits;

    //get code
    return c + 2*xbits + (shifted>=2?2:0);
}

void addDistanceXbits(iBitstream &bs, uint16_t distance) {
    //shift length into range
    uint16_t shifted = distance-1;

    uint16_t xbits = getDistanceXbits(distance);

    uint16_t r1 = shifted % (1<<xbits);

    //add extra bits
    bs.pushRL(r1,xbits);
}




















void literalStatic(iBitstream &bs, uint16_t literal) {
    if (literal<=143) {
        bs.pushLR(48+literal,8);
    }else{
        bs.pushLR(400+(literal-144),9);
    }
}

void addLengthCode(iBitstream &bs, uint16_t length) {
    //edge case 258
    if (length == 258) {
        lengthStatic(bs, 285);
        return;
    }

    uint16_t code = getLengthCode(length);

    //add static length code
    lengthStatic(bs, code);

    //add extra bits
    addLengthXbits(bs, length);
}

void lengthStatic(iBitstream &bs, uint16_t lengthCode) {
    if (lengthCode <= 279) {
        bs.pushLR((lengthCode-256),7);
    }else{
        bs.pushLR(192+(lengthCode-280),8);
    }
}

void addDistanceCode(iBitstream &bs, uint16_t distance) {
    uint16_t code = getDistanceCode(distance);

    //add static distance code
    distanceStatic(bs, code);

    addDistanceXbits(bs, distance);
}

void distanceStatic(iBitstream &bs, uint16_t distanceCode) {
    bs.pushLR(distanceCode, 5);
}



vector<uint8_t> huffman_static(vector<Code> codes, uint32_t adler) {
    vector<uint8_t> vec;

    //flags
    uint8_t cmf = 0x78;
    uint8_t flg = 0x40;
    vec.push_back(cmf);
    vec.push_back(flg+FCheck(cmf,flg));

    //bitstream
    iBitstream bs(BitOrder::LSBitFirst);

    //header
    bs.pushLR(static_cast<uint8_t>(0x06),3); //last block, static codes

    //push data to bitstream
    for (Code c : codes) {
        switch (c.type) {
            case CodeType::Literal:
                literalStatic(bs,c.val);
                break;
            case CodeType::Length:
                addLengthCode(bs,c.val);
                break;
            case CodeType::Distance:
                addDistanceCode(bs,c.val);
                break;
            default: break;
        }
    }

    //end of block
    lengthStatic(bs,256);

    //add data to byte vector
    vector<uint8_t> data = bs.getBytesMove();
    for (uint i : data) {
        vec.push_back(i);
    }

    //adler
    pushInt(vec, adler);

    return vec;
}









vector<pair<uint8_t,uint8_t>> codeLengthEncoding(vector<uint8_t> &lengths) {
    vector<pair<uint8_t,uint8_t>> encoding;

    size_t i=0;
    while (i<lengths.size()) {
        //count 0 series (cod 17/18)
        size_t j=i;
        while (j<lengths.size() && j-i<138 && lengths[j]==0) ++j;
        if (j-i>=11) {
            encoding.emplace_back(18,j-i-11);
            i=j;
            continue;
        }else if (j-i>=3) {
            encoding.emplace_back(17,j-i-3);
            i=j;
            continue;
        }

        //repeating series (code 16)
        if (i>0) {
            size_t j=i;
            while (j<lengths.size() && j-i<6 && lengths[j]==lengths[i-1]) ++j;
            if (j-i>=3) {
                encoding.emplace_back(16,j-i-3);
                i=j;
                continue;
            }
        }

        //add length code 0-15
        encoding.emplace_back(lengths[i],0);
        ++i;
    }

    return encoding;
}

void addLiteralDynamic(iBitstream &bs, uint16_t code, vector<DynamicCode> &codes) {
    bs.pushLR(codes[code].val,codes[code].length);
}

void addLengthCode(iBitstream &bs, uint16_t length, vector<DynamicCode> &codes) {
    if (length == 258) {
        bs.pushLR(codes[285].val,codes[285].length);
        return;
    }

    uint16_t code = getLengthCode(length);

    bs.pushLR(codes[code].val,codes[code].length);

    addLengthXbits(bs,length);
}

void addDistanceCode(iBitstream &bs, uint16_t distance, vector<DynamicCode> &codes) {
    uint16_t code = getDistanceCode(distance);

    bs.pushLR(codes[code].val,codes[code].length);

    addDistanceXbits(bs,distance);
}


vector<uint8_t> huffman_dynamic(vector<Code> codes, uint32_t adler) {
    vector<uint8_t> vec;

    //flags
    uint8_t cmf = 0x78;
    uint8_t flg = 0x40;
    vec.push_back(cmf);
    vec.push_back(flg+FCheck(cmf,flg));

    //bitstream
    iBitstream bs(BitOrder::LSBitFirst);

    //header
    bs.pushLR(static_cast<uint8_t>(0x05),3); //last block, dynamic codes

    //get frequencies
    vector<size_t> LLfrequencies(288, 0);
    vector<size_t> distFrequencies(32,0);
    for (size_t i=0; i<codes.size(); ++i) {
        if (codes[i].type == CodeType::Literal) {
            ++LLfrequencies[codes[i].val];
        }else if (codes[i].type == CodeType::Length) {
            ++LLfrequencies[getLengthCode(codes[i].val)];
        }else if (codes[i].type == CodeType::Distance) {
            ++distFrequencies[getDistanceCode(codes[i].val)];
        }
    }
    //end of block
    ++LLfrequencies[256];

    //create prefix codes for Literals&length and for distances
    vector<DynamicCode> LLCodes = getPrefixCodes(LLfrequencies, 15);
    vector<DynamicCode> distCodes = getPrefixCodes(distFrequencies,15);

    //cut off 0-LL codes
    size_t LLCodesSize = LLCodes.size();
    while(LLCodesSize>257 && LLCodes[LLCodesSize-1].length==0) --LLCodesSize;

    //cut off 0-dist codes
    size_t distCodesSize = distCodes.size();
    while(distCodesSize>1 && distCodes[distCodesSize-1].length==0) --distCodesSize;

    //combine length and distance lengths for code length codes
    vector<uint8_t> allCodeLengths;
    for (size_t i=0;i<LLCodesSize;++i) {
        allCodeLengths.push_back(LLCodes[i].length);
    }
    for (size_t i=0;i<distCodesSize;++i) {
        allCodeLengths.push_back(distCodes[i].length);
    }

    //get basic encoding for length in 0-18 alphabet
    vector<pair<uint8_t,uint8_t>> basicEncoding = codeLengthEncoding(allCodeLengths);

    //get frequencies of code length codes
    vector<size_t> CLCfrequencies(19,0);
    for (size_t i=0;i<basicEncoding.size();++i) {
        ++CLCfrequencies[basicEncoding[i].first];
    }

    //create prefix codes for code length codes
    vector<DynamicCode> CLCCodes = getPrefixCodes(CLCfrequencies,7);

    //cut off 0-CL codes
    size_t CLCodeSize = CLCCodes.size();
    while(CLCodeSize>4 && CLCCodes[CLCOrder[CLCodeSize-1]].length==0) --CLCodeSize;

    //add count bits
    bs.pushRL(LLCodesSize-257,5); //HLIT
    bs.pushRL(distCodesSize-1,5); //HDIST
    bs.pushRL(CLCodeSize-4,4); //HCLEN

    //output code length code lengths
    for (size_t i=0; i<CLCodeSize;++i) {
        bs.pushRL(CLCCodes[CLCOrder[i]].length,3);
    }

    //output huffman coded code lengths for literals and length/distances
    for (size_t i=0;i<basicEncoding.size();++i) {
        bs.pushLR(CLCCodes[basicEncoding[i].first].val,CLCCodes[basicEncoding[i].first].length);
        //extra bits
        if (basicEncoding[i].first==18) {
            bs.pushRL(basicEncoding[i].second,7);
        }else if (basicEncoding[i].first==17) {
            bs.pushRL(basicEncoding[i].second,3);
        }else if (basicEncoding[i].first==16) {
            bs.pushRL(basicEncoding[i].second,2);
        }
    }

    //output huffman coded data for this block
     for (Code c : codes) {
        switch (c.type) {
            case CodeType::Literal:
                addLiteralDynamic(bs,c.val,LLCodes);
                break;
            case CodeType::Length:
                addLengthCode(bs,c.val,LLCodes);
                break;
            case CodeType::Distance:
                addDistanceCode(bs,c.val,distCodes);
                break;
            default: break;
        }
    }

    //end of block
    addLiteralDynamic(bs,256,LLCodes);

    //add data to byte vector
    vector<uint8_t> data = bs.getBytesMove();
    for (uint i : data) {
        vec.push_back(i);
    }

    //adler
    pushInt(vec, adler);

    return vec;
}









