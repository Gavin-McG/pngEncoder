#ifndef IMAGEINFO_TPP
#define IMAGEINFO_TPP

#include "imageInfo.h"

template<typename T>
ImageInfo<T>::ImageInfo(uint32_t width, uint32_t height, ColorType colorType) : width(width), height(height), bitDepth(sizeof(T)*8), colorType(colorType) {
    filters.resize(height);
    ref.resize(height);
    for (size_t i=0; i<ref.size(); ++i) {
        ref[i].resize(width);
    }
}

template<typename T>
ImageInfo<T>::ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color) : width(width), height(height), bitDepth(sizeof(T)*8), colorType(colorType) {
    filters.resize(height);
    ref.resize(height);
    for (size_t i=0; i<ref.size(); ++i) {
        ref[i].resize(width, color);
    }
}

template<typename T>
void ImageInfo<T>::setFilters(FilterType type) {
    for (size_t i=0; i<filters.size(); ++i) {
        filters[i] = type;
    }
}

template<typename T>
void ImageInfo<T>::calculateBestFilters() {

}

template<typename T>
size_t ImageInfo<T>::getScanlineSize() {
    switch (colorType) {
        case ColorType::Grey:
            return 1+1*width*bitDepth/8;
        case ColorType::True:
            return 1+3*width*bitDepth/8;
        case ColorType::Indexed:
            cerr << "Indexed ColorType not supported\n";
            return 0;
        case ColorType::GreyAlpha:
            return 1+2*width*bitDepth/8;
        case ColorType::TrueAlpha:
            return 1+4*width*bitDepth/8;
        default:
            cerr << "Invalid ColorType " << colorType << '\n';
            return 0;
    }
}

template<typename T>
vector<uint8_t> ImageInfo<T>::getScanline(size_t row, FilterType type) {
    vector<uint8_t> vec;
    vec.reserve(getScanlineSize());
    vec.push_back(static_cast<uint8_t>(type));
    for (size_t i=0; i<width; ++i) {
        vector<T> x = ref[row][i].getbytes<T>(colorType);
        vector<T> a, b, c;
        switch (type) {
            case FilterType::None:
                for (size_t j=0; j<x.size(); ++j) {
                    pushInt(vec,static_cast<T>(x[j]));
                }
                break;
            case FilterType::Sub:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    pushInt(vec,static_cast<T>(x[j]-a[j]));
                }
                break;
            case FilterType::Up:
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    pushInt(vec,static_cast<T>(x[j]-b[j]));
                }
                break;
            case FilterType::Average:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    pushInt(vec,static_cast<T>(x[j]-((b[j]+a[j])>>1)));
                }
                break;
            case FilterType::Paeth:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                c = (i==0 || row==0 ? Color(0,0,0,0) : ref[row-1][i-1]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    pushInt(vec,static_cast<T>(x[j]-paethPredictor(a[j],b[j],c[j])));
                }
                break;
            default:
                break;
        }
        
    }
    return vec;
}

template<typename T>
vector<uint8_t> ImageInfo<T>::getDatastream() {
    vector<uint8_t> vec;
    vec.reserve(getScanlineSize()*height);
    for (size_t i=0; i<height; ++i) {
        vector<uint8_t> scanline = getScanline(i,filters[i]);
        for (size_t j=0; j<scanline.size(); ++j) {
            vec.push_back(scanline[j]);
        }
    }
    return vec;
}

#endif