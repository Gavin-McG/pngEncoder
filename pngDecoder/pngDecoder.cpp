#include <fstream>
#include <iostream>
#include <getopt.h>
#include <stdint.h>
#include <iomanip>

#include "../include/imageInfo.h"

using namespace std;

struct Options {
    string fileIn;
    string fileOut;
};

void getMode(int argc, char * argv[], Options &options) {
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    option long_options[] = {
        { "input",  required_argument,    nullptr, 'i'},
        { "output",  required_argument,    nullptr, 'o'},
        { nullptr, 0, nullptr, '\0' }
    };  // long_options[]

    while ((choice = getopt_long(argc, argv, "i:o:", long_options, &index)) != -1) {
        switch (choice) {
            case 'i':
                options.fileIn = optarg;
                break;
            case 'o':
                options.fileOut = optarg;
                break;
            default:
                cerr << "Error: invalid option" << "\n";
                exit(1);
        }  // switch ..choice
    }

    //default file name
    if (options.fileIn == "") options.fileIn = "default.png";
    if (options.fileOut == "") options.fileOut = "default.png";
}

int main(int argc, char* argv[]) {
    Options options;
    getMode(argc, argv, options);

    //decode and re-encode
    if (true) {
        ifstream fs(options.fileIn, ios::binary);
        ImageInfo image(fs);
        fs.close();

        //image.colorType = ColorType::TrueAlpha;

        ofstream fs2(options.fileOut);
        image.printPng(fs2,DeflateType::NoCompression);
        fs2.close();
    }

    //simple encode
    if (false) {
        ImageInfo image(30,30,Color(1,0,0));
        ofstream fs(options.fileOut);
        image.printPng(fs,DeflateType::NoCompression);
        fs.close();
    }

    //simple decode
    if (false) {
        ifstream fs(options.fileIn);
        ImageInfo image(fs);
        fs.close();
    }

    return 0;
}