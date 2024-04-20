#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include <string>
#include <math.h>
#include <vector>

#include "../include/imageInfo.h"


//TODO:

//Dynamic coding
//Lz77 hash optimization
//bit depth 1-2-4
//Indexed color
//More drawing functions
//meta-data
//Interlacing


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

    //1x1 image test
    if (true) {
        ofstream fs(options.fileOut);

        ImageInfo image(1,1,ColorType::True,Color(1,0,0));

        image.printPng(fs);

        fs.close();
    }

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
    if (false) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo image(200,200,ColorType::Grey,Color(0.1,0.7,0.9,1));
        image.setFilters(FilterType::Paeth);

        for (float i=0; i<200; i++) {
            for (float j=0; j<200; ++j) {
                image.drawPixel(i,j,Color(i/200,j/200,0.5,1));
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

    //filter test
    if (false) {
        //output
        ofstream fs(options.fileOut);

        ImageInfo image(200,200,ColorType::Grey,BitDepth::Eight,Color(0.1,0.7,0.9,1));
        image.setFilters(FilterType::Average);

        for (float i=0; i<200; i++) {
            for (float j=0; j<200; ++j) {
                if ((static_cast<int>(i+j)/5)%2==0) image.drawPixel(i,j,Color(i/200,j/200,0.5,1));
            }
        }

        //ImageInfo kernel = getGaussian(9,3);
        vector<vector<float>> vec = {{0,-1,0},{-1,4,-1},{0,-1,0}};
        ImageInfo kernel(vec);
        
        ImageInfo filtered = image.filter(kernel,false);

        filtered.printPng(fs,DeflateType::NoCompression);
        
        fs.close();
    }

    return 0;
}





