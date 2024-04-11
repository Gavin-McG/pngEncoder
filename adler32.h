#ifndef ADLER32_H
#define ADLER32_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

const uint32_t MOD_ADLER = 65521;

uint32_t getADLER32(stringstream &ss);
uint32_t getADLER32(string s);
uint32_t getADLER32(vector<uint8_t> &vec);


#endif