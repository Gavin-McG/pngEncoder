#ifndef IMAGEINFO_TPP
#define IMAGEINFO_TPP

#include "imageInfo.h"

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

#endif