#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include <string>
#include "utilities.h"
#include "crc32.h"
#include "imageInfo.h"
#include "huffman.h"
#include "adler32.h"
#include "lz77.h"
#include "deflate.h"

using namespace std;

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

    // Get the mode from the command line
    Options options;
    getMode(argc, argv, options);

    //simple image test
    if (false) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo image(2000,2000,ColorType::True,Color(0.8,0.4,0.1,1));
        image.setFilters(FilterType::Paeth);
        
        image.printPng(fs);
        
        fs.close();
    }

    //static tests
    if (true) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo image(100,100,ColorType::True,Color(0.1,0.7,0.9,1));
        image.setFilters(FilterType::Paeth);

        for (float i=0; i<100; i++) {
            for (float j=0; j<100; ++j) {
                if (uint(i+j)%2==0) image.drawPixel(i,j,Color(i/100,j/100,0.5,1));
            }
        }
        
        image.printPng(fs);
        
        fs.close();
    }

    //lz77 debug test
    if (false) {
        vector<uint8_t> vec = {0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff};
        lz77_compress(vec);
    }

    return 0;
}





