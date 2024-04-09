#ifndef CHUNKS_H
#define CHUNKS_H

#include <iostream>
#include <string>
#include <sstream>
#include <stdint.h>
#include "utilities.h"
#include "crc32.h"
#include "imageInfo.h"

using namespace std;

//png chunks

//print the signature of a png file to an ostream
//constnt 8 bytes
void printSig(ostream &os);

//prints an IDHR header chunk to an ostream from an imageinfo object
//length is always 13 bytes
template<typename T>
void printIDHR(ostream &os, const ImageInfo<T> &info, const uint32_t* crcTable);

//prints the IEND chunk to an ostream
//constant 12 bytes, but uses CRC32 algorithm because why not?
void printIEND(ostream &os, const uint32_t* crcTable);

#include "chunks.tpp"

#endif