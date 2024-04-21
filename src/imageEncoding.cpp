#include "../include/imageInfo.h"





void ImageInfo::calculateBestFilters() {

}

size_t ImageInfo::getScanlineSize() const {
    switch (colorType) {
        case ColorType::Grey:
            return 1+1*width*bitDepth/8;
        case ColorType::True:
            return 1+3*width*bitDepth/8;
        case ColorType::Indexed:
            cerr << "Indexed ColorType not supported\n";
            return 0;
        case ColorType::GreyAlpha:
            return 1+2*width*bitDepth/8;
        case ColorType::TrueAlpha:
            return 1+4*width*bitDepth/8;
        default:
            cerr << "Invalid ColorType " << colorType << '\n';
            return 0;
    }
}


vector<uint8_t> ImageInfo::getDatastream() const {
    vector<uint8_t> vec;
    vec.reserve(getScanlineSize()*height);

    switch (bitDepth) {
        case BitDepth::One:
            
            break;
        case BitDepth::Two:
            
            break;
        case BitDepth::Four:
            
            break;
        case BitDepth::Eight:
            for (size_t i=0; i<height; ++i) {
                vector<uint8_t> scanline = getScanline<uint8_t>(i,filters[i]);
                for (size_t j=0; j<scanline.size(); ++j) {
                    vec.push_back(scanline[j]);
                }
            }
            break;
        case BitDepth::Sixteen:
            for (size_t i=0; i<height; ++i) {
                vector<uint8_t> scanline = getScanline<uint16_t>(i,filters[i]);
                for (size_t j=0; j<scanline.size(); ++j) {
                    vec.push_back(scanline[j]);
                }
            }
            break;
        default: break;
    }
    
    return vec;
}









void ImageInfo::printPng(ostream &os, DeflateType deflateType) const {
    if (!crcGenerated) generateCRC(crcTable);

    printSig(os);
    printIDHR(os);
    printIDAT(os, deflateType);
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

void ImageInfo::printIDAT(ostream &os, DeflateType deflateType) const {
    //get and compress data
    vector<uint8_t> data = getDatastream();
    vector<uint8_t> compressed = deflate(data, deflateType);
    
    //get chunk name and data to compute crc32
    stringstream ss;
    ss << "IDAT";
    for (uint8_t i : compressed) {
        ss << i;
    }

    //clear compressed to save memory
    uint32_t length = compressed.size();
    compressed = vector<uint8_t>();

    //compute crc32
    uint32_t crc = getCRC32(ss,crcTable);

    //chunk data length
    printInt(os,length);
    //name and data
    os << ss.str();
    //crc32
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