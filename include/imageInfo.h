#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <stdint.h>
#include <math.h>
#include <vector>
#include "color.h"
#include "utilities.h"
#include "deflate.h"
#include "crc32.h"

using namespace std;

static const ColorType  DEFAULT_COLORTYPE   = ColorType::True;
static const BitDepth   DEFAULT_BITDEPTH    = BitDepth::Eight;
static const Color      DEFAULT_COLOR       = Color();

//class used to store info of an image.
class ImageInfo {
    inline static uint32_t crcTable[256];
    inline static bool crcGenerated = false;

    public:
    uint32_t width, height;
    BitDepth bitDepth;
    ColorType colorType;

    //constructors (width, height + )
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType = ColorType::True, BitDepth bitdepth = BitDepth::Eight, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, BitDepth bitdepth, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, const Color &color);

    //conversion constructor
    template<typename T>
    requires integral<T> || floating_point<T>
    ImageInfo(vector<vector<T>> values, ColorType colorType = ColorType::True, BitDepth bitDepth = BitDepth::Eight);
    template<typename T>
    requires integral<T> || floating_point<T>
    ImageInfo(vector<vector<T>> values, BitDepth bitDepth);

    //copy constructor
    ImageInfo(const ImageInfo &other);

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

    //critical png components
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