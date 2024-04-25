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



template<typename T>
vector<uint8_t> ImageInfo::getScanline(size_t row, FilterType type) const {
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








template <typename T>
void ImageInfo::reverseScanline(size_t row) {
    for (size_t i=0; i<width; ++i) {
        vector<T> x = ref[row][i].getbytes<T>(colorType);
        vector<T> a, b, c;
        vector<T> vec;
        switch (filters[row]) {
            case FilterType::None:
                break;
            case FilterType::Sub:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    vec.push_back(x[j]+a[j]);
                }
                break;
            case FilterType::Up:
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    vec.push_back(x[j]+b[j]);
                }
                break;
            case FilterType::Average:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    vec.push_back(x[j]+((b[j]+a[j])>>1));
                }
                break;
            case FilterType::Paeth:
                a = (i==0 ? Color(0,0,0,0) : ref[row][i-1]).getbytes<T>(colorType);
                b = (row==0 ? Color(0,0,0,0) : ref[row-1][i]).getbytes<T>(colorType);
                c = (i==0 || row==0 ? Color(0,0,0,0) : ref[row-1][i-1]).getbytes<T>(colorType);
                for (size_t j=0; j<x.size(); ++j) {
                    vec.push_back(x[j]+paethPredictor(a[j],b[j],c[j]));
                }
                break;
            default:
                break;
        }
        if (vec.size()==1 || vec.size()==3) vec.push_back(UINT8_MAX);
        ref[row][i] = Color(vec)/UINT8_MAX;
    }
}



#endif