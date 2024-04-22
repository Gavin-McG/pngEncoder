#include <fstream>
#include <iostream>
#include <getopt.h>
#include <stdint.h>
#include <iomanip>

#include "../include/imageInfo.h"

using namespace std;

struct Options {
    string fileIn;
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
                options.fileIn = optarg;
                break;

            default:
                cerr << "Error: invalid option" << "\n";
                exit(1);
        }  // switch ..choice
    }

    //default file name
    if (options.fileIn == "") options.fileIn = "default.png";
}

int main(int argc, char* argv[]) {
    Options options;
    getMode(argc, argv, options);

    ifstream fs(options.fileIn, ios::binary);

    ImageInfo image(fs);
    fs.close();

    cout << image.width << ' ' << image.height << endl;
    cout << static_cast<int>(image.bitDepth) << ' ' << static_cast<int>(image.colorType) << endl;

    return 0;
}