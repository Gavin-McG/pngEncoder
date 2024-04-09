#ifndef CHUNKS_TPP
#define CHUNKS_TPP

#include "chunks.h"

template<typename T>
void printIDHR(ostream &os, const ImageInfo<T> &info, const uint32_t* crcTable) {
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

#endif