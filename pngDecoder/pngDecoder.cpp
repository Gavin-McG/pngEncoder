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

    //static decode test
    if (false) {
        random_device rd;  
        mt19937 gen(rd());
        uniform_int_distribution<> distribution(0, 255); 

        vector<uint8_t> inputData;
        for (size_t i=0;i<100; ++i) {
            inputData.push_back(distribution(gen));
        }
        for (size_t i=0;i<100;++i) {
            inputData.push_back(inputData[i]);
        }

        cout << "input data:" << endl;
        for (size_t i=0; i<inputData.size(); ++i) {
            cout << setw(3) << static_cast<int>(inputData[i]) << ' ';
        }
        cout << endl << endl;

        vector<Code> inputCodes = lz77_compress(inputData);

        cout << "input codes:" << endl;
        for (size_t i=0; i<inputCodes.size(); ++i) {
            if (inputCodes[i].type==Literal) cout << 'L';
            else if (inputCodes[i].type==Length) cout << 'P';
            else if (inputCodes[i].type==Distance) cout << 'D';

            cout << static_cast<int>(inputCodes[i].val) << ' ';
        }
        cout << endl << endl;

        vector<uint8_t> encodedData = huffman_static(inputCodes,0);

        cout << "encoded data:" << endl;
        for (size_t i=0; i<encodedData.size(); ++i) {
            for (size_t j=8; j>0; --j) {
                cout << static_cast<int>((encodedData[i]>>(j-1))%2);
            }
            cout << ' ';
        }
        cout << endl << endl;

        oBitstream bs(encodedData);
        vector<Code> outputCodes;
        bs.getRL<uint8_t>(19); //get rid of unnecessary data for test
        decompress_static(bs,outputCodes);

        cout << "outut codes:" << endl;
        for (size_t i=0; i<outputCodes.size(); ++i) {
            if (outputCodes[i].type==Literal) cout << 'L';
            else if (outputCodes[i].type==Length) cout << 'P';
            else if (outputCodes[i].type==Distance) cout << 'D';

            cout << static_cast<int>(outputCodes[i].val) << ' ';
        }
        cout << endl << endl;

        vector<uint8_t> outputdata;
        lz77_decompress(outputCodes,outputdata);

        for (size_t i=0;i<outputdata.size();++i) {
            cout << setw(3) << static_cast<int>(outputdata[i]) << ' ';
        }
        cout << endl << endl;

        bool passed = true;
        if (outputdata.size() != inputData.size()) {
            passed = false;
        }else{
            for (size_t i=0; i<inputData.size(); ++i) {
                if (inputData[i]!=outputdata[i]) {
                    passed = false;
                    break;
                }
            }
        }
        if (passed) {
            cout << "Test was passed" << endl;
        }else{
            cout << "Test failed" << endl;
        }
    }

    return 0;
}