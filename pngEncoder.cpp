#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include <string>
#include "chunks.h"
#include "utilities.h"
#include "crc32.h"
#include "imageInfo.h"
#include "huffman.h"
#include "adler32.h"

using namespace std;

uint32_t crcTable[256];

struct Options {
    string fileOut;
};

void getMode(int argc, char * argv[], Options &options) {
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    option long_options[] = {
        { "output",  required_argument,    nullptr, 'o'},
        { nullptr, 0, nullptr, '\0' }
    };  // long_options[]

    while ((choice = getopt_long(argc, argv, "o:", long_options, &index)) != -1) {
        switch (choice) {
            case 'o':
                options.fileOut = optarg;
                break;

            default:
                cerr << "Error: invalid option" << "\n";
                exit(1);
        }  // switch ..choice
    }

    //default file name
    if (options.fileOut == "") options.fileOut = "default.png";
}

int main(int argc, char* argv[]) {
    //generate CRC32 lookup table
    generateCRC(crcTable);

    // Get the mode from the command line
    Options options;
    getMode(argc, argv, options);
    
    //info of the image
    ImageInfo<uint8_t> info(1,1,ColorType::True);

    //output
    ofstream fs(options.fileOut);

    /*
    //chunks
    printSig(fs);
    printIDHR(fs,info,crcTable);

    //example 1x1 IDAT chunk
    printInt(fs,uint32_t(12)); //length
    fs << "IDAT"; //chunk name
    fs << "\x08"; //deflate method
    fs << "\xd7"; //ZLIB Fcheck
    fs << "\x63\xF8\xCF\xC0\x00\x00"; //deflate block
    fs << "\x03\x01\x01\x00"; //ADLER32 check
    fs << "\x18\xDD\x8D\xB0"; //CRC32 checksum

    printIEND(fs,crcTable);

    //close png file
    fs.close();
    */

    ImageInfo<uint8_t> image(2000,2000,ColorType::True,Color(0.8,0.4,0.1,1));
    image.setFilters(FilterType::Sub);
    
    printSig(fs);
    printIDHR(fs,image,crcTable);

    vector<uint8_t> data = image.getDatastream();
    uint32_t adler = getADLER32(data);
    vector<uint8_t> compressed = huffman_uncompressed(data, adler);
    data.clear();
    
    stringstream ss;
    ss << "IDAT";
    for (uint8_t i : compressed) {
        ss << i;
    }
    uint32_t crc = getCRC32(ss,crcTable);

    uint32_t length = compressed.size();
    printInt(fs,length);
    fs << ss.str();
    printInt(fs,crc);

    cout << length;

    printIEND(fs, crcTable);
    fs.close();

    return 0;
}





