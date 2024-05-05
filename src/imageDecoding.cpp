#include "../include/imageInfo.h"

ImageInfo::ImageInfo(istream &is, const DebugOptions &options) {
    if (!crcGenerated) generateCRC(crcTable);

    //verify signature
    if (!verifySig(is)) {
        cerr << "PNG signature not correctly identified" << endl;
        cerr << "\tFile in either corrupted or is not in PNG format" << endl;
        *this = ImageInfo();
        return;
    }

    //read chunks until end chunk is found
    vector<uint8_t> literals;
    while (!readChunk(is, literals, options));

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
                for (size_t j=1; j<bytes[i].size(); ++j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    bytes[i][j] += a;
                }
                break;
            case FilterType::Up:
                for (size_t j=1; j<bytes[i].size(); ++j) {
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] += b;
                }
                break;
            case FilterType::Average:
                for (size_t j=1; j<bytes[i].size(); ++j) {
                    a = (j<=byteShift)?0:bytes[i][j-byteShift];
                    b = (i==0)?0:bytes[i-1][j];
                    bytes[i][j] += ((b+a)>>1);
                }
                break;
            case FilterType::Paeth:
                for (size_t j=1; j<bytes[i].size(); ++j) {
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
    uint8_t filterVal = bs.getLR<uint8_t>(8);
    filters[row]=static_cast<FilterType>(filterVal);
    if (filterVal>4) {
        cerr << "Invalid filter number " << static_cast<int>(filterVal) << " in row " << row << endl;
    }

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


bool ImageInfo::readChunk(istream &is, vector<uint8_t> &literals, const DebugOptions &options) {
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
        cerr << "CRC mismatch is chunk " << chunkName << endl;
        cerr << "\tread CRC: " << fileCRC << endl;
        cerr << "\tcaluculated CRC: " << generatedCRC << endl;
    }

    //run chunk-specific computation
    if (chunkName == "IHDR") {
        readIHDR(data);
    }else if (chunkName == "IDAT") {
        readIDAT(data, literals, options);
    }else if (chunkName == "IEND") {
        readIEND(data);
    }else{
        cerr << "skipping over " << chunkName << " chunk" << endl;
    }
    
    //return
    return chunkName == "IEND";
}




void ImageInfo::readIHDR(vector<uint8_t> &data) {
    if (data.size() != 13) cerr << "Inavlid length in IHDR chunk" << endl;

    oBitstream bs(move(data),BitOrder::MSBitFirst);
    width = bs.getLR<uint32_t>(32);
    height = bs.getLR<uint32_t>(32);
    bitDepth = static_cast<BitDepth>(bs.getLR<uint8_t>(8));
    colorType = static_cast<ColorType>(bs.getLR<uint8_t>(8));
}




void ImageInfo::readIDAT(vector<uint8_t> &data, vector<uint8_t> &literals, const DebugOptions &options) {
    oBitstream bs(move(data));

    uint8_t CMF = bs.getRL<uint8_t>(8);
    uint8_t FLG = bs.getRL<uint8_t>(8);

    vector<Code> codes = huffman_decompress(bs, options.huffmanDebug);

    lz77_decompress(codes, literals, options.lz77Debug);

    bs.setOrder(BitOrder::MSBitFirst);
    uint32_t fileAdler = bs.getLR<uint32_t>(32);
    uint32_t generatedAdler = getADLER32(literals);

    if (fileAdler != generatedAdler) {
        cerr << "ADLER32 mismatch in decompressed data" << endl;
        cerr << "\tread Adler: " << fileAdler << endl;
        cerr << "\tcaluculated Adler: " << generatedAdler << endl;
        cerr << "\t# of literals: " << literals.size() << endl;
        cerr << "\t# of codes: " << codes.size() << endl;
    }
}




void ImageInfo::readIEND(vector<uint8_t> &data) {
    if (data.size() != 0) cerr << "Inavlid length in IEND chunk" << endl;
}