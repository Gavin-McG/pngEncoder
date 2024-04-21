#include "../include/imageInfo.h"

ImageInfo::ImageInfo(istream &is) {
    if (!crcGenerated) generateCRC(crcTable);

    //defult image if sig isn't verified
    if (!verifySig(is)) {
        *this = ImageInfo();
        cout << "incorrect png signature" << endl;
        return;
    }

    while(!readChunk(is));
}





bool ImageInfo::verifySig(istream &is) {
    const char* sig = reinterpret_cast<const char*>("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a");
    char readSig[8];
    is.read(readSig,8);
    return strcmp(readSig,sig)==0;
}


bool ImageInfo::readChunk(istream &is) {
    //read length
    uint32_t length = readInt<uint32_t>(is);
    
    //read chunk name
    string chunkName = "";
    for (uint8_t i=0;i<4;++i) {
        char c;
        is >> c;
        chunkName += c;
    }

    //read chunk data
    char* data = new char[length];
    is.read(data, length);

    //get crc
    uint32_t fileCRC = readInt<uint32_t>(is);
    uint32_t generatedCRC = getCRC32(chunkName, crcTable);
    generatedCRC = getCRC32(data, length, crcTable, generatedCRC);

    if (fileCRC != generatedCRC) {
        cout << "CRC mismatch is chunk " << chunkName << endl;
        cout << "read CRC: " << fileCRC << endl;
        cout << "caluculated CRC: " << generatedCRC << endl;
    }

    delete[] data;
    return chunkName == "IEND";
}




HeaderInfo ImageInfo::readIDHR(istream &is) {
    HeaderInfo info;

    is >> info.width;
    is >> info.height;
    is >> info.bitDepth;
    is >> info.colorType;
    is >> info.compressionMethod;
    is >> info.filterMethod;
    is >> info.interlaceMethod;

    return info;
}