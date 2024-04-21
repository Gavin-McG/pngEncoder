#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <stdint.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <cstring>
#include "color.h"
#include "utilities.h"
#include "deflate.h"
#include "crc32.h"

using namespace std;

static const ColorType  DEFAULT_COLORTYPE   = ColorType::True;
static const BitDepth   DEFAULT_BITDEPTH    = BitDepth::Eight;
static const Color      DEFAULT_COLOR       = Color();

struct HeaderInfo {
    uint32_t width;
    uint32_t height;
    uint8_t bitDepth;
    uint8_t colorType;
    uint8_t compressionMethod;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
    uint8_t crc;
};

struct IDATInfo {
    vector<uint8_t> dataStream;
    uint32_t crc;
};

//class used to store info of an image.
class ImageInfo {
    inline static uint32_t crcTable[256];
    inline static bool crcGenerated = false;

    public:
    uint32_t width, height;
    BitDepth bitDepth;
    ColorType colorType;

    //default constructor
    ImageInfo();

    //constructors (width, height + )
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType = DEFAULT_COLORTYPE, BitDepth bitdepth = DEFAULT_BITDEPTH, const Color &color = DEFAULT_COLOR);
    ImageInfo(uint32_t width, uint32_t height, BitDepth bitdepth, const Color &color = DEFAULT_COLOR);
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color = DEFAULT_COLOR);
    ImageInfo(uint32_t width, uint32_t height, const Color &color);

    //conversion constructor
    template<typename T>
    requires integral<T> || floating_point<T>
    ImageInfo(vector<vector<T>> values, ColorType colorType = DEFAULT_COLORTYPE, BitDepth bitDepth = DEFAULT_BITDEPTH);
    template<typename T>
    requires integral<T> || floating_point<T>
    ImageInfo(vector<vector<T>> values, BitDepth bitDepth);

    //constructor from png file
    ImageInfo(istream &is);

    //copy constructor
    ImageInfo(const ImageInfo &other);

    //assignment operator
    ImageInfo& operator=(const ImageInfo &other);

    //filter functions
    void setFilters(FilterType type);
    void calculateBestFilters();

    //output encoded png file to ostream
    void printPng(ostream &os, DeflateType deflateType = DeflateType::StaticCodes) const;

    //drawing functions
    void drawPixel(uint32_t x, uint32_t y, const Color &color);
    void drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color &color);

    //used for debugging
    Color getPixel(uint32_t x, uint32_t y);

    //image filters
    ImageInfo filter(const ImageInfo &kernel, bool normalize = false) const;

    private:
    vector<vector<Color>> ref;
    vector<FilterType> filters;

    //raw data functions
    size_t getScanlineSize() const;
    template<typename T>
    vector<uint8_t> getScanline(size_t row, FilterType type) const;
    vector<uint8_t> getDatastream() const;

    //reading png components from istream
    static bool verifySig(istream &is);
    bool readChunk(istream &is);
    HeaderInfo readIDHR(istream &is);
    IDATInfo readIDAT(istream &is);
    void readIEND(istream &is);
    void readUnknown(istream &is);

    //encoding critical png components
    void printSig(ostream &os) const;
    void printIDHR(ostream &os) const;
    void printIDAT(ostream &os, DeflateType deflateType) const;
    void printIEND(ostream &os) const;

    friend ImageInfo getGaussian(size_t N, float sigma);
};

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);


ImageInfo getGaussian(size_t N, float sigma);

#include "imageInfo.tpp"

#endif