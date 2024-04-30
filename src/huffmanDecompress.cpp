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







bool checkCode(const DynamicCode &code, vector<Code> &codes) {
    if (code.length==7 && code.val>=0 && code.val<=23) {
        return true;
    }else if (code.length==8 && code.val>=48 && code.val<=199) {
        return true;
    }else if (code.length==9 && code.val>=400 && code.val<=511) {
        return true;
    }
    return false;
}

void decompress_static(oBitstream &bs, vector<Code> &codes) {
    while (true) {
        DynamicCode readCode = DynamicCode(7,bs.getLR<uint16_t>(7));
        
    }
}









void decompress_dynamic(oBitstream &bs, vector<Code> &codes) {
}