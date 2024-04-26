#ifndef IMAGEINFO_TPP
#define IMAGEINFO_TPP

#include "imageInfo.h"



template<typename T>
requires integral<T> || floating_point<T>
ImageInfo::ImageInfo(vector<vector<T>> values, ColorType colorType, BitDepth bitDepth) {
    if (colorType != ColorType::Grey && (bitDepth < 8)) {
        cerr << "Inavlid colorType+bitdepth combination" << endl;
    }

    width = values[0].size();
    height = values.size();
    this->colorType = colorType;
    this->bitDepth = bitDepth;

    filters.resize(height);
    ref.resize(height);
    for (size_t i=0; i<ref.size(); ++i) {
        ref[i].reserve(width);
        for (size_t j=0; j<values[i].size() && j<width; ++j) {
            ref[i].emplace_back(values[i][j],values[i][j]);
        }
        ref[i].resize(width,Color(0,0));
    }
}

template<typename T>
requires integral<T> || floating_point<T>
ImageInfo::ImageInfo(vector<vector<T>> values, BitDepth bitDepth) :
    ImageInfo(values, DEFAULT_COLORTYPE, bitDepth) {}












#endif