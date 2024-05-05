#include <fstream>
#include <iostream>
#include <getopt.h>
#include <stdint.h>
#include <iomanip>
#include <random>

#include "../include/imageInfo.h"

using namespace std;

struct Options {
    string fileIn;
    string fileOut;
    DebugOptions debugOpt;
};

void getMode(int argc, char * argv[], Options &options) {
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    option long_options[] = {
        { "input",          required_argument,  nullptr, 'i'},
        { "output",         required_argument,  nullptr, 'o'},
        { "debug-huffman",  no_argument,        nullptr, 'h'},
        { "debug-lz77",     no_argument,        nullptr, 'l'},
        { "debug-filters",  no_argument,        nullptr, 'f'},
        { "debug",          no_argument,        nullptr, 'd'},
        { nullptr, 0, nullptr, '\0' }
    };  // long_options[]

    while ((choice = getopt_long(argc, argv, "i:o:hlfd", long_options, &index)) != -1) {
        switch (choice) {
            case 'i':
                options.fileIn = optarg;
                break;
            case 'o':
                options.fileOut = optarg;
                break;
            case 'h':
                options.debugOpt.huffmanDebug = true;
                break;
            case 'l':
                options.debugOpt.lz77Debug = true;
                break;
            case 'f':
                options.debugOpt.filterDebug = true;
                break;
            case 'd':
                options.debugOpt.filterDebug = true;
                options.debugOpt.lz77Debug = true;
                options.debugOpt.huffmanDebug = true;
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



ImageInfo getTest1() {
    //create image
    size_t w=500, h=500;
    ImageInfo image(w,h,ColorType::True,BitDepth::Eight,Color(0.2,0.6,0.3));
    
    for (size_t i=0; i<w; ++i) {
        for (size_t j=0; j<h; ++j) {
            if ((w/2-i)*(w/2-i)+(h/2-j)*(h/2-j)<40000) {
                image.drawPixel(i,j, Color(1,0.6,0.35));
            }
            if ((i/5)%2 != (j/5)%2) {
                image.drawPixel(i,j,Color(0.02*(i%50),0.02*(j%50),0.02*(i*j%50)));
            }
        }
    }

    ImageInfo gaussian = getGaussian(5,1);
    ImageInfo newImage = image.filter(gaussian);

    return image;
}



int main(int argc, char* argv[]) {
    Options options;
    getMode(argc, argv, options);

    //generate variants
    if (true) {
        //image suffixes
        vector<FilterType> filters = {FilterType::None, FilterType::Sub, FilterType::Up, FilterType::Average, FilterType::Paeth};
        vector<string> filterSuffix = {"n","s","u","a","p"};

        vector<DeflateType> compMethods = {DeflateType::NoCompression, DeflateType::StaticCodes, DeflateType::DynamicCodes};
        vector<string> compMethodSuffix = {"n","s","d"};

        ImageInfo image = getTest1();

        //encode versions
        for (size_t i=0; i<filters.size(); ++i) {
            for (size_t j=0; j<compMethods.size(); ++j) {
                image.setFilters(filters[i]);

                ofstream fs(options.fileOut + '-' + filterSuffix[i] + compMethodSuffix[j] + ".png");
                image.printPng(fs, compMethods[j], options.debugOpt);
                fs.close();
            }
        }
    }

    //read variants to basic non-compression
    if (false) {
        vector<FilterType> filters = {FilterType::None, FilterType::Sub, FilterType::Up, FilterType::Average, FilterType::Paeth};
        vector<string> filterSuffix = {"n","s","u","a","p"};

        vector<DeflateType> compMethods = {DeflateType::NoCompression, DeflateType::StaticCodes, DeflateType::DynamicCodes};
        vector<string> compMethodSuffix = {"n","s","d"};

        for (size_t i=0; i<filters.size(); ++i) {
            for (size_t j=0; j<compMethods.size(); ++j) {
                ifstream ifs(options.fileOut + '-' + filterSuffix[i] + compMethodSuffix[j] + ".png");
                ImageInfo image(ifs);
                ifs.close();

                ofstream ofs(options.fileOut + '-' + filterSuffix[i] + compMethodSuffix[j] + "-out.png");
                image.printPng(ofs,DeflateType::DynamicCodes);
                ofs.close();
            }
        }
    }




    //generate 1 image
    if (false) {
        ImageInfo image = getTest1();

        ofstream fs(options.fileOut);
        image.printPng(fs, DeflateType::StaticCodes, options.debugOpt);
        fs.close();
    }

    //read 1 image
    if (false) {
        ifstream ifs(options.fileIn);
        ImageInfo image(ifs, options.debugOpt);
        ifs.close();
    }

    //read & print image
    if (false) {
        //read image
        ifstream ifs(options.fileIn);
        ImageInfo image(ifs, options.debugOpt);
        ifs.close();

        //filtering
        cout << "filtering..." << endl;
        ImageInfo gauss = getGaussian(25,5);
        for (size_t i=0;i<0;++i) {
            cout << "applying filter " << i << endl;
            image = image.filterBilaterial(gauss,0.02,true);
        }
        cout << "done." << endl;

        //print image
        ofstream fs(options.fileOut);
        image.printPng(fs, DeflateType::DynamicCodes, options.debugOpt);
        fs.close();
    }

    return 0;
}