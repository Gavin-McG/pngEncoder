#ifndef CRC32_H
#define CRC32_H

#include <string>
#include <sstream>
#include "utilities.h"

using namespace std;

//polynomial used for CRC table hash
const uint32_t CRC_POLY = 0xEDB88320;
//default starting value when generating crc value
const uint32_t CRC_INITIAL = 0xffffffff;

//generate a lookup table for a crc32 a checksum
void generateCRC(uint32_t* crcTable);

//get the CRC32 checksum from a stringstream and a provided lookup-table
uint32_t getCRC32(stringstream &ss, const uint32_t* crcTable, bool flip = true, uint32_t initial = CRC_INITIAL);
//get the CRC32 checksum from a string and a provided lookup-table
uint32_t getCRC32(const string &s, const uint32_t* crcTable, bool flip = true, uint32_t initial = CRC_INITIAL);

uint32_t getCRC32(const char* s, size_t length, const uint32_t* crcTable, bool flip = true, uint32_t initial = CRC_INITIAL);

uint32_t getCRC32(const vector<uint8_t> &bytes, const uint32_t* crcTable, bool flip, uint32_t initial = CRC_INITIAL);


uint32_t iterateCRC(uint32_t crc, uint8_t v, const uint32_t* crcTable);


#endif