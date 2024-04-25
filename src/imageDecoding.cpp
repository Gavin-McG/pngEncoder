#include "../include/imageInfo.h"

ImageInfo::ImageInfo(istream &is) {
    if (!crcGenerated) generateCRC(crcTable);

    //defult image if sig isn't verified
    if (!verifySig(is)) {
        *this = ImageInfo();
        cout << "incorrect png signature" << endl;
        return;
    }

    //get data from all chunks
    vector<uint8_t> literals;
    while(!readChunk(is, literals));

    int expected = height * getScanlineSize();
    if (literals.size() != expected) {
        cout << "number of literals doesn't match with expected bytes" << endl;
        cout << "\texpeded: " << expected << endl;
        cout << "\tread: " << literals.size() << endl;
    }

    filters.resize(height);
    ref = vector<vector<Color>>(height,vector<Color>(width));

    //pack literals into colors
    setImage(literals);
}









void ImageInfo::setImage(vector<uint8_t> &literals) {
    for (size_t i=0;i<height;++i) {
        setScanline(i, literals);
    }
    reverseFilters();
}

void ImageInfo::setScanline(size_t row, vector<uint8_t> &literals) {
    cout << "writing row: " << row << endl;
    size_t rowSize = getScanlineSize();
    size_t rowStart = rowSize*row;

    filters[row] = static_cast<FilterType>(literals[rowStart]);
    if (literals[rowStart]>4) {
        cout << "Inavlid filter value found" << endl;
    }

    switch(colorType) {
        case ColorType::Grey:
            for (size_t i=0;i<width;++i) {
                uint8_t v = literals[rowStart+i+1];
                ref[row][i] = Color(v)/UINT8_MAX;
            }
            break;
        case ColorType::True:
            for (size_t i=0;i<width;++i) {
                uint8_t r = literals[rowStart+3*i+1];
                uint8_t g = literals[rowStart+3*i+2];
                uint8_t b = literals[rowStart+3*i+3];
                ref[row][i] = Color(r,g,b)/UINT8_MAX;
            }
            break;
        case ColorType::Indexed:
            cout << "indexed coming soon" << endl;
            break;
        case ColorType::GreyAlpha:
            for (size_t i=0;i<width;++i) {
                uint8_t v = literals[rowStart+2*i+1];
                uint8_t a = literals[rowStart+2*i+2];
                ref[row][i] = Color(v,a)/UINT8_MAX;
            }
            break;
        case ColorType::TrueAlpha:
            for (size_t i=0;i<width;++i) {
                uint8_t r = literals[rowStart+4*i+1];
                uint8_t g = literals[rowStart+4*i+2];
                uint8_t b = literals[rowStart+4*i+3];
                uint8_t a = literals[rowStart+4*i+4];
                ref[row][i] = Color(r,g,b,a)/UINT8_MAX;
            }
            break;
        default:
            cout << "invalid colorType" << endl;
            break;
    }
}


void ImageInfo::reverseFilters() {
    for (uint32_t i=0;i<height; ++i) {
        reverseScanline<uint8_t>(i);
    }
}










bool ImageInfo::verifySig(istream &is) {
    const char* sig = reinterpret_cast<const char*>("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a");
    char readSig[8];
    is.read(readSig,8);
    return strcmp(readSig,sig)==0;
}


bool ImageInfo::readChunk(istream &is, vector<uint8_t> &literals) {
    //check that is is still valid
    if (is.eof()) return true;

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
    uint32_t generatedCRC = getCRC32(chunkName, crcTable, false);
    generatedCRC = getCRC32(data, length, crcTable, true, generatedCRC);

    if (fileCRC != generatedCRC) {
        cout << "CRC mismatch is chunk " << chunkName << endl;
        cout << "\tread CRC: " << fileCRC << endl;
        cout << "\tcaluculated CRC: " << generatedCRC << endl;
    }

    //run chunk-specific computation
    if (chunkName == "IHDR") {
        cout << "reading IHDR chunk..." << endl; 
        readIHDR(data, length);
    }else if (chunkName == "IDAT") {
        cout << "reading IDAT chunk..." << endl; 
        readIDAT(data, length, literals);
    }else if (chunkName == "IEND") {
        cout << "reading IEND chunk..." << endl;
        readIEND(data, length);
    }else{
        cout << "skipping over " << chunkName << " chunk" << endl;
    }
    
    //deallocate and return
    delete[] data;
    return chunkName == "IEND";
}




void ImageInfo::readIHDR(char* data, size_t length) {
    if (length != 13) cout << "Inavlid length in IHDR chunk" << endl;

    width = packInt<uint32_t>(data);
    height = packInt<uint32_t>(data+4);
    bitDepth = static_cast<BitDepth>(packInt<uint8_t>(data+8));
    colorType = static_cast<ColorType>(packInt<uint8_t>(data+9));
}




void ImageInfo::readIDAT(char* data, size_t length, vector<uint8_t> &literals) {
    uint8_t CMF = packInt<uint8_t>(data);
    uint8_t FLG = packInt<uint8_t>(data);

    vector<Code> codes = huffman_decompress(data+2, length-6);

    for (size_t i=0;i<codes.size();++i) {
        cout << codes[i].val << " ";
    }
    cout << endl;

    lz77_decompress(codes, literals);

    uint32_t fileAdler = packInt<uint32_t>(data+length-4);
    uint32_t generatedAdler = getADLER32(literals);

    if (fileAdler != generatedAdler) {
        cout << "ADLER32 mismatch in decompressed data" << endl;
        cout << "\tread Adler: " << fileAdler << endl;
        cout << "\tcaluculated Adler: " << generatedAdler << endl;
        cout << "\t# of literals: " << literals.size() << endl;
        cout << "\t# of codes: " << codes.size() << endl;
        cout << '\t' << static_cast<int>(literals[0]) << endl;
    }
}




void ImageInfo::readIEND(char* data, size_t length) {
    if (length != 0) cout << "Inavlid length in IEND chunk" << endl;
}