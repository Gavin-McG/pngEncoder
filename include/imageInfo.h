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
#include "bitstream.h"

using namespace std;

//TODO:

//Dynamic coding
//Lz77 hash optimization
//Indexed color
//More drawing functions
//meta-data
//Interlacing

enum FilterType : uint8_t {
    None,
    Sub,
    Up,
    Average,
    Paeth
};

enum ColorType : uint8_t {
    Grey = 0,
    True = 2,
    Indexed = 3,
    GreyAlpha = 4,
    TrueAlpha = 6
};

enum BitDepth : uint8_t {
    One = 1,
    Two = 2,
    Four = 4,
    Eight = 8,
    Sixteen = 16
};

static const ColorType  DEFAULT_COLORTYPE   = ColorType::True;
static const BitDepth   DEFAULT_BITDEPTH    = BitDepth::Eight;
static const Color      DEFAULT_COLOR       = Color();

static const uint8_t UINT4_MAX = 15;
static const uint8_t UINT2_MAX = 3;
static const uint8_t UINT1_MAX = 1;

struct DebugOptions {
    bool filterDebug = false;
    bool lz77Debug = false;
    bool huffmanDebug = false;

    DebugOptions() : filterDebug(false), lz77Debug(false), huffmanDebug(false) {}
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
    ImageInfo(istream &is, const DebugOptions &options = DebugOptions());

    //copy constructor
    ImageInfo(const ImageInfo &other);

    //assignment operator
    ImageInfo& operator=(const ImageInfo &other);

    //filter functions
    void setFilters(FilterType type);
    void calculateBestFilters();

    //output encoded png file to ostream
    void printPng(ostream &os, DeflateType deflateType = DeflateType::StaticCodes, const DebugOptions &options = DebugOptions()) const;

    //drawing functions
    void drawPixel(uint32_t x, uint32_t y, const Color &color);
    void drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color &color);

    //used for debugging
    Color getPixel(uint32_t x, uint32_t y);

    //complex image manipulation
    ImageInfo filter(const ImageInfo &kernel, bool normalize = true) const;
    ImageInfo filterBilaterial(const ImageInfo &kernel, float s, bool normalize = true) const;
    void clamp01();
    void applyGreyscale();
    void setOpacity(float f);

    private:
    vector<vector<Color>> ref;
    vector<FilterType> filters;

    
    //decoding helper functions
    vector<vector<uint8_t>> unflattenBytes(vector<uint8_t> &bytes) const;
    void unfilterBytes(vector<vector<uint8_t>> &bytes) const;
    void setScanline(size_t row, vector<uint8_t> &bytes);
    void setImageBytes(vector<vector<uint8_t>> &bytes);

    //reading png components from istream
    static bool verifySig(istream &is);
    bool readChunk(istream &is, vector<uint8_t> &literals, const DebugOptions &options = DebugOptions());
    void readIHDR(vector<uint8_t> &data);
    void readIDAT(vector<uint8_t> &data, vector<uint8_t> &literals, const DebugOptions &options = DebugOptions());
    void readIEND(vector<uint8_t> &data);
    void readUnknown(vector<uint8_t> &data);

    //encoding helper functions
    vector<uint8_t> getScanline(size_t row) const;
    vector<vector<uint8_t>> getImageBytes() const;
    void filterBytes(vector<vector<uint8_t>> &bytes) const;
    vector<uint8_t> flattenBytes(vector<vector<uint8_t>> &bytes) const;

    //encoding critical png components
    void printSig(ostream &os) const;
    void printIDHR(ostream &os) const;
    void printIDAT(ostream &os, DeflateType deflateType, const DebugOptions &options = DebugOptions()) const;
    void printIEND(ostream &os) const;

    friend ImageInfo getGaussian(size_t N, float sigma);
};

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);


ImageInfo getGaussian(size_t N, float sigma);

#include "imageInfo.tpp"

#endif