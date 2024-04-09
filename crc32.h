#ifndef CRC32_H
#define CRC32_H

#include <string>
#include <sstream>
#include "utilities.h"

using namespace std;

//generate a lookup table for a crc32 a checksum
void generateCRC(uint32_t* crcTable);

//get the CRC32 checksum from a stringstream and a provided lookup-table
uint32_t getCRC32(stringstream &ss, const uint32_t* crcTable);
//get the CRC32 checksum from a string and a provided lookup-table
uint32_t getCRC32(const string &s, const uint32_t* crcTable);


#endif