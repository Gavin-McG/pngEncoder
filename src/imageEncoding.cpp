#include "../include/imageInfo.h"




vector<uint8_t> ImageInfo::getScanline(size_t row) const {
    iBitstream bs(BitOrder::MSBitFirst);

    //compute #bits and max value from bit-depth
    size_t bits = static_cast<size_t>(bitDepth);
    uint16_t maxVal = (1ul << bits) - 1;

    //push filter
    bs.pushLR(static_cast<uint8_t>(filters[row]),8);

    //add all pixels in row to bitstream
    uint16_t v,r,g,b,a;
    switch (colorType) {
        case ColorType::Grey:
            for (size_t i=0; i<width; ++i) {
                v = static_cast<uint16_t>(clamp(ref[row][i].getGreyscale(),0.0f,1.0f)*maxVal);
                bs.pushLR(v,bits);
            }
            break;
        case ColorType::True:
            for (size_t i=0; i<width; ++i) {
                r = static_cast<uint16_t>(clamp(ref[row][i].r,0.0f,1.0f)*maxVal);
                g = static_cast<uint16_t>(clamp(ref[row][i].g,0.0f,1.0f)*maxVal);
                b = static_cast<uint16_t>(clamp(ref[row][i].b,0.0f,1.0f)*maxVal);
                bs.pushLR(r,bits);
                bs.pushLR(g,bits);
                bs.pushLR(b,bits);
            }
            break;
        case ColorType::Indexed:

            break;
        case ColorType::GreyAlpha:
            for (size_t i=0; i<width; ++i) {
                v = static_cast<uint16_t>(clamp(ref[row][i].getGreyscale(),0.0f,1.0f)*maxVal);
                a = static_cast<uint16_t>(clamp(ref[row][i].a,0.0f,1.0f)*maxVal);
                bs.pushLR(v,bits);
                bs.pushLR(a,bits);
            }
            break;
        case ColorType::TrueAlpha:
            for (size_t i=0; i<width; ++i) {
                r = static_cast<uint16_t>(clamp(ref[row][i].r,0.0f,1.0f)*maxVal);
                g = static_cast<uint16_t>(clamp(ref[row][i].g,0.0f,1.0f)*maxVal);
                b = static_cast<uint16_t>(clamp(ref[row][i].b,0.0f,1.0f)*maxVal);
                a = static_cast<uint16_t>(clamp(ref[row][i].a,0.0f,1.0f)*maxVal);
                bs.pushLR(r,bits);
                bs.pushLR(g,bits);
                bs.pushLR(b,bits);
                bs.pushLR(a,bits);
            }
            break;
        default: break;
    }
    
    return bs.getBytesMove();
}

vector<vector<uint8_t>> ImageInfo::getImageBytes() const {
    vector<vector<uint8_t>> table(height);
    for (size_t i=0; i<height; ++i) {
        table[i] = getScanline(i);
    }
    return table;
}

void ImageInfo::filterBytes(vector<vector<uint8_t>> &bytes) const {
    int byteShift = 0;
    if (colorType == ColorType::Grey) byteShift = 1;
    else if (colorType == ColorType::GreyAlpha) byteShift = 2;
    else if (colorType == ColorType::True) byteShift = 3;
    else if (colorType == ColorType::TrueAlpha) byteShift = 4;
    byteShift *= static_cast<int>(bitDepth)/8;
    byteShift = max(1,byteShift);

    for (int i=height-1; i>=0; --i) {
        uint8_t a,b,c;
        switch (filters[i]) {
            case FilterType::None:
                break;
            case FilterType::Sub:
                for (size_t j=bytes[i].size()-1; j>0; --j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    bytes[i][j] -= a;
                }
                break;
            case FilterType::Up:
                for (size_t j=bytes[i].size()-1; j>0; --j) {
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] -= b;
                }
                break;
            case FilterType::Average:
                for (size_t j=bytes[i].size()-1; j>0; --j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] -= ((b+a)>>1);
                }
                break;
            case FilterType::Paeth:
                for (size_t j=bytes[i].size()-1; j>0; --j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    b = (i==0)?0:bytes[i-1][j];
                    c = (i==0||j<=byteShift)?0:bytes[i-1][j-byteShift];
                    bytes[i][j] -= paethPredictor(a,b,c);
                }
                break;
            default: break;
        }
    }
}

vector<uint8_t> ImageInfo::flattenBytes(vector<vector<uint8_t>> &bytes) const {
    vector<uint8_t> result;
    result.reserve(bytes.size()*bytes[0].size());
    for (size_t i=0; i<bytes.size(); ++i) {
        for (size_t j=0; j<bytes[i].size();++j) {
            result.push_back(bytes[i][j]);
        }
        bytes[i] = vector<uint8_t>();
    }
    bytes = vector<vector<uint8_t>>();
    return result;
}











void ImageInfo::printPng(ostream &os, DeflateType deflateType, const DebugOptions &options) const {
    if (!crcGenerated) generateCRC(crcTable);

    printSig(os);
    printIDHR(os);
    printIDAT(os, deflateType, options);
    printIEND(os);
}

void ImageInfo::printSig(ostream &os) const {
    os << "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
}

void ImageInfo::printIDHR(ostream &os) const {
    //length of chunk
    printInt(os, uint32_t(13)); //length

    //add name and datastream to ss
    stringstream ss;
    ss << "IHDR";
    printInt(ss, width); //width
    printInt(ss, height); //height
    printInt(ss, bitDepth); //bit depth
    printInt(ss, static_cast<uint8_t>(colorType)); //color type
    printInt(ss, uint8_t(0)); //compression method
    printInt(ss, uint8_t(0)); //filter method
    printInt(ss, uint8_t(0)); //interlacing

    //add checksum of length and datastream to ss
    printInt(ss,getCRC32(ss, crcTable));

    //output ss to ostream
    os << ss.str();
}

void ImageInfo::printIDAT(ostream &os, DeflateType deflateType, const DebugOptions &options) const {
    //get image data
    vector<vector<uint8_t>> bytes = getImageBytes();
    filterBytes(bytes);
    vector<uint8_t> data = flattenBytes(bytes);

    //calculate adler32
    uint32_t adler = getADLER32(data);

    //huffman coding
    vector<uint8_t> datastream;
    if (deflateType==DeflateType::NoCompression) {
        //huffmanno compression
        datastream = huffman_uncompressed(data, adler);
    }else if (deflateType==DeflateType::StaticCodes) {
        //compress into codes
        vector<Code> codes = lz77_compress(data, options.lz77Debug);
        data = vector<uint8_t>();
        //huffman static coding
        datastream = huffman_static(codes,adler);
    }else{
        //compress into codes
        vector<Code> codes = lz77_compress(data, options.lz77Debug);
        data = vector<uint8_t>();
        //huffman dynamic coding
        datastream = huffman_dynamic(codes,adler);
    }

    //output data
    printInt(os, uint32_t(datastream.size()));
    string chunkName = "IDAT";
    os << chunkName;
    for (size_t i=0;i<datastream.size();++i) {
        printInt(os,datastream[i]);
    }

    //crc
    uint32_t crc = getCRC32(chunkName,crcTable,false);
    crc = getCRC32(datastream,crcTable,true,crc);
    printInt(os,crc);
}

void ImageInfo::printIEND(ostream &os) const {
    printInt(os, static_cast<uint32_t>(0));
    string chunkName = "IEND";
    os << chunkName;
    printInt(os,getCRC32(chunkName, crcTable));
}








uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    int16_t p = static_cast<int16_t>(a) + static_cast<int16_t>(b) - static_cast<int16_t>(c);
    int16_t pa = abs(p - static_cast<int16_t>(a));
    int16_t pb = abs(p - static_cast<int16_t>(b));
    int16_t pc = abs(p - static_cast<int16_t>(c));
    if (pa <= pb && pa <= pc) return static_cast<uint8_t>(a);
    else if (pb <= pc) return static_cast<uint8_t>(b);
    return static_cast<uint8_t>(c);
}