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
#include "lz77.h"

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

    //cout << ios_base::sync_with_stdio(false);

    //generate CRC32 lookup table
    generateCRC(crcTable);

    // Get the mode from the command line
    Options options;
    getMode(argc, argv, options);

    //simpleimage test
    if (false) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo<uint8_t> image(2000,2000,ColorType::True,Color(0.8,0.4,0.1,1));
        image.setFilters(FilterType::Sub);
        
        printSig(fs);
        printIDHR(fs,image,crcTable);

        vector<uint8_t> data = image.getDatastream();
        uint32_t adler = getADLER32(data);
        vector<uint8_t> compressed = huffman_uncompressed(data, adler);
        uint32_t length = compressed.size();
        data = vector<uint8_t>();
        
        stringstream ss;
        ss << "IDAT";
        for (uint8_t i : compressed) {
            ss << i;
        }
        compressed = vector<uint8_t>();
        uint32_t crc = getCRC32(ss,crcTable);

        printInt(fs,length);
        fs << ss.str();
        printInt(fs,crc);

        printIEND(fs, crcTable);
        fs.close();
    }

    //lz77 test
    if (false) {
        ImageInfo<uint8_t> image(16,16,ColorType::TrueAlpha,Color(0.6,0.4,1,1));

        vector<uint8_t> data = image.getDatastream();

        vector<Code> encoding = lz77_compress(data);

        for (Code c : encoding) {
            switch(c.type) {
                case CodeType::Literal:
                    cout << 'V';
                    break;
                case CodeType::Length:
                    cout << 'L';
                    break;
                case CodeType::Distance:
                    cout << 'D';
                    break;
                default: break;
            }
            cout << c.val << ' ';
        }
    }

    //static tests
    if (true) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo<uint8_t> image(100,100,ColorType::True,Color(1,0,0,1));
        image.setFilters(FilterType::None);
        
        printSig(fs);
        printIDHR(fs,image,crcTable);

        vector<uint8_t> data = image.getDatastream();
        uint32_t adler = getADLER32(data);
        vector<Code> codes = lz77_compress(data);
        cout << "finished lz77" << endl;
        data = vector<uint8_t>();
        vector<uint8_t> compressed = huffman_static(codes, adler);
        uint32_t length = compressed.size();
        cout << "finished static" << endl;
        codes = vector<Code>();
        
        stringstream ss;
        ss << "IDAT";
        for (uint8_t i : compressed) {
            ss << i;
        }
        compressed = vector<uint8_t>();
        uint32_t crc = getCRC32(ss,crcTable);

        printInt(fs,length);
        fs << ss.str();
        printInt(fs,crc);

        printIEND(fs, crcTable);
        fs.close();
    }

    //lz77 debug test
    if (false) {
        vector<uint8_t> vec = {0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff};
        lz77_compress(vec);
    }

    return 0;
}





