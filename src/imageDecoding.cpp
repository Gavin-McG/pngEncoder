#include "../include/imageInfo.h"

ImageInfo::ImageInfo(istream &is) {
    if (!crcGenerated) generateCRC(crcTable);

    //verify signature
    if (!verifySig(is)) {
        cout << "PNG signature not correctly identified" << endl;
        cout << "\tFile in either corrupted or is not in PNG format" << endl;
        *this = ImageInfo();
        return;
    }

    //read chunks until end chunk is found
    vector<uint8_t> literals;
    while (!readChunk(is, literals));

    //set vectors
    ref = vector<vector<Color>>(height,vector<Color>(width));
    filters = vector<FilterType>(height);
    
    //read data into image
    vector<vector<uint8_t>> data = unflattenBytes(literals);
    literals = vector<uint8_t>();
    unfilterBytes(data);

    setImageBytes(data);
}


vector<vector<uint8_t>> ImageInfo::unflattenBytes(vector<uint8_t> &bytes) const {
    vector<vector<uint8_t>> result;
    result.resize(height);

    cout << "decompression complete" << endl;
    cout << "\tRead " << bytes.size() << " bytes" << endl;

    size_t rowSize = bytes.size()/height;
    for (size_t i=0; i<height; ++i) {
        result[i] = vector<uint8_t>(bytes.begin()+(i*rowSize),bytes.begin()+((i+1)*rowSize));
    }

    return result;
}

void ImageInfo::unfilterBytes(vector<vector<uint8_t>> &bytes) const {
    int byteShift = 0;
    if (colorType == ColorType::Grey) byteShift = 1;
    else if (colorType == ColorType::GreyAlpha) byteShift = 2;
    else if (colorType == ColorType::True) byteShift = 3;
    else if (colorType == ColorType::TrueAlpha) byteShift = 4;
    byteShift *= static_cast<int>(bitDepth)/8;
    byteShift = max(1,byteShift);

    for (int i=0; i<height; ++i) {
        uint8_t a,b,c;
        switch (static_cast<FilterType>(bytes[i][0])) {
            case FilterType::None:
                break;
            case FilterType::Sub:
                for (size_t j=1; j<bytes.size(); ++j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    bytes[i][j] += a;
                }
                break;
            case FilterType::Up:
                for (size_t j=1; j<bytes.size(); ++j) {
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] += b;
                }
                break;
            case FilterType::Average:
                for (size_t j=1; j<bytes.size(); ++j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] += ((b+a)>>1);
                }
                break;
            case FilterType::Paeth:
                for (size_t j=1; j<bytes.size(); ++j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    b = (i==0)?0:bytes[i-1][j];
                    c = (i==0||j<=byteShift)?0:bytes[i-1][j-byteShift];
                    bytes[i][j] += paethPredictor(a,b,c);
                }
                break;
            default: break;
        }
    }
}


void ImageInfo::setScanline(size_t row, vector<uint8_t> &bytes) {
    oBitstream bs(move(bytes),BitOrder::MSBitFirst);

    //compute #bits and max value from bit-depth
    size_t bits = static_cast<size_t>(bitDepth);
    uint16_t maxVal = (1ul << bits) - 1;

    //set filter
    filters[row]=static_cast<FilterType>(bs.getLR<uint8_t>(8));

    //add all pixels in row to bitstream
    uint16_t v,r,g,b,a;
    switch (colorType) {
        case ColorType::Grey:
            for (size_t i=0; i<width; ++i) {
                v = bs.getLR<uint16_t>(bits);
                ref[row][i] = Color(v,maxVal)/maxVal;
            }
            break;
        case ColorType::True:
            for (size_t i=0; i<width; ++i) {
                r = bs.getLR<uint16_t>(bits);
                g = bs.getLR<uint16_t>(bits);
                b = bs.getLR<uint16_t>(bits);
                ref[row][i] = Color(r,g,b,maxVal)/maxVal;
            }
            break;
        case ColorType::Indexed:

            break;
        case ColorType::GreyAlpha:
            for (size_t i=0; i<width; ++i) {
                v = bs.getLR<uint16_t>(bits);
                a = bs.getLR<uint16_t>(bits);
                ref[row][i] = Color(v,a)/maxVal;
            }
            break;
        case ColorType::TrueAlpha:
            for (size_t i=0; i<width; ++i) {
                r = bs.getLR<uint16_t>(bits);
                g = bs.getLR<uint16_t>(bits);
                b = bs.getLR<uint16_t>(bits);
                a = bs.getLR<uint16_t>(bits);
                ref[row][i] = Color(r,g,b,a)/maxVal;
            }
            break;
        default: break;
    }
}

void ImageInfo::setImageBytes(vector<vector<uint8_t>> &bytes) {
    for (size_t i=0; i<height; ++i) {
        setScanline(i, bytes[i]);
    }
}











bool ImageInfo::verifySig(istream &is) {
    const char* sig = reinterpret_cast<const char*>("\x89\x50\x4e\x47\x0d\x0a\x1a\x0a");
    char readSig[9];
    is.read(readSig,8);
    readSig[8] = '\0';
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
    vector<uint8_t> data(length);
    is.read(reinterpret_cast<char*>(data.data()), length);

    //get crc
    uint32_t fileCRC = readInt<uint32_t>(is);
    uint32_t generatedCRC = getCRC32(chunkName, crcTable, false);
    generatedCRC = getCRC32(data, crcTable, true, generatedCRC);

    if (fileCRC != generatedCRC) {
        cout << "CRC mismatch is chunk " << chunkName << endl;
        cout << "\tread CRC: " << fileCRC << endl;
        cout << "\tcaluculated CRC: " << generatedCRC << endl;
    }

    //run chunk-specific computation
    if (chunkName == "IHDR") {
        cout << "reading IHDR chunk..." << endl; 
        readIHDR(data);
    }else if (chunkName == "IDAT") {
        cout << "reading IDAT chunk..." << endl; 
        readIDAT(data, literals);
    }else if (chunkName == "IEND") {
        cout << "reading IEND chunk..." << endl;
        readIEND(data);
    }else{
        cout << "skipping over " << chunkName << " chunk" << endl;
    }
    
    //return
    return chunkName == "IEND";
}




void ImageInfo::readIHDR(vector<uint8_t> &data) {
    if (data.size() != 13) cout << "Inavlid length in IHDR chunk" << endl;

    oBitstream bs(move(data),BitOrder::MSBitFirst);
    width = bs.getLR<uint32_t>(32);
    height = bs.getLR<uint32_t>(32);
    bitDepth = static_cast<BitDepth>(bs.getLR<uint8_t>(8));
    colorType = static_cast<ColorType>(bs.getLR<uint8_t>(8));
}




void ImageInfo::readIDAT(vector<uint8_t> &data, vector<uint8_t> &literals) {
    oBitstream bs(move(data));

    uint8_t CMF = bs.getRL<uint8_t>(8);
    uint8_t FLG = bs.getRL<uint8_t>(8);

    vector<Code> codes = huffman_decompress(bs);

    // for (size_t i=0;i<codes.size();++i) {
    //     cout << codes[i].val << " ";
    // }
    // cout << endl;

    lz77_decompress(codes, literals);

    bs.setOrder(BitOrder::MSBitFirst);
    uint32_t fileAdler = bs.getLR<uint32_t>(32);
    uint32_t generatedAdler = getADLER32(literals);

    if (fileAdler != generatedAdler) {
        cout << "ADLER32 mismatch in decompressed data" << endl;
        cout << "\tread Adler: " << fileAdler << endl;
        cout << "\tcaluculated Adler: " << generatedAdler << endl;
        cout << "\t# of literals: " << literals.size() << endl;
        cout << "\t# of codes: " << codes.size() << endl;
    }
}




void ImageInfo::readIEND(vector<uint8_t> &data) {
    if (data.size() != 0) cout << "Inavlid length in IEND chunk" << endl;
}