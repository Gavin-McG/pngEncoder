#include "chunks.h"

void printSig(ostream &os) {
    os << "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
}

void printIDHR(ostream &os, const ImageInfo &info, const uint32_t* crcTable) {
    //length of chunk
    printInt(os, uint32_t(13)); //length

    //add name and datastream to ss
    stringstream ss;
    ss << "IHDR";
    printInt(ss, info.width); //width
    printInt(ss, info.height); //height
    printInt(ss, info.bitDepth); //bit depth
    printInt(ss, static_cast<uint8_t>(info.colorType)); //color type
    printInt(ss, uint8_t(0)); //compression method
    printInt(ss, uint8_t(0)); //filter method
    printInt(ss, uint8_t(0)); //interlacing

    //add checksum of length and datastream to ss
    printInt(ss,getCRC32(ss, crcTable));

    //output ss to ostream
    os << ss.str();
}

void printIDAT(ostream &os, const ImageInfo &info, DeflateType deflateType, const uint32_t* crcTable) {
    //get and compress data
    vector<uint8_t> data = info.getDatastream();
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

void printIEND(ostream &os, const uint32_t* crcTable) {
    printInt(os, static_cast<uint32_t>(0));
    os << "IEND";
    printInt(os,getCRC32("IEND", crcTable));
}