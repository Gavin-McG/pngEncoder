#include <iostream>
#include <iomanip>
#include <fstream>
#include <getopt.h>
#include <string>
#include <math.h>
#include <vector>

#include "../include/imageInfo.h"
#include "../include/dynamicTree.h"



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

    

    return 0;
}





