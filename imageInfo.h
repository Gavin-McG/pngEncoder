#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <stdint.h>
#include "color.h"
#include "utilities.h"

using namespace std;

//class used to store info of an image.
template<typename T>
class ImageInfo {
    public:
    uint32_t width, height;
    uint8_t bitDepth;
    ColorType colorType;

    ImageInfo(uint32_t width, uint32_t height, ColorType colorType);
    ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color);

    void setFilters(FilterType type);
    void calculateBestFilters();

    size_t getScanlineSize();
    vector<uint8_t> getScanline(size_t row, FilterType type);
    vector<uint8_t> getDatastream();

    private:
    vector<vector<Color>> ref;
    vector<FilterType> filters;
};

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);

#include "imageInfo.tpp"

#endif