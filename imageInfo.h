#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <stdint.h>
#include "color.h"
#include "utilities.h"

using namespace std;

static const ColorType  DEFAULT_COLORTYPE   = ColorType::True;
static const BitDepth   DEFAULT_BITDEPTH    = BitDepth::Eight;
static const Color      DEFAULT_COLOR       = Color();

//class used to store info of an image.
class ImageInfo {
    public:
    uint32_t width, height;
    BitDepth bitDepth;
    ColorType colorType;

    ImageInfo(uint32_t width, uint32_t height, ColorType colorType = ColorType::True, BitDepth bitdepth = BitDepth::Eight, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, BitDepth bitdepth, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color = Color());
    ImageInfo(uint32_t width, uint32_t height, const Color &color);

    void setFilters(FilterType type);
    void calculateBestFilters();

    size_t getScanlineSize() const;

    template<typename T>
    vector<uint8_t> getScanline(size_t row, FilterType type) const;
    vector<uint8_t> getDatastream() const;

    private:
    vector<vector<Color>> ref;
    vector<FilterType> filters;
};

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);

#include "imageInfo.tpp"

#endif