#include "chunks.h"

void printSig(ostream &os) {
    os << "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
}

void printIEND(ostream &os, const uint32_t* crcTable) {
    printInt(os, static_cast<uint32_t>(0));
    os << "IEND";
    printInt(os,getCRC32("IEND", crcTable));
}