#ifndef COLOR_TPP
#define COLOR_TPP

#include "color.h"

template<typename T>
vector<T> Color::getbytes(ColorType colorType) {
    vector<T> vec;
    switch (colorType) {
        case ColorType::Grey:
            vec.push_back(tobytes<T>(getGreyscale()));
            break;
        case ColorType::True:
            vec.push_back(tobytes<T>(r));
            vec.push_back(tobytes<T>(g));
            vec.push_back(tobytes<T>(b));
            break;
        case ColorType::Indexed:
            cerr << "Indexed ColorType not supported\n";
            break;
        case ColorType::GreyAlpha:
            vec.push_back(tobytes<T>(getGreyscale()));
            vec.push_back(tobytes<T>(a));
            break;
        case ColorType::TrueAlpha:
            vec.push_back(tobytes<T>(r));
            vec.push_back(tobytes<T>(g));
            vec.push_back(tobytes<T>(b));
            vec.push_back(tobytes<T>(a));
            break;
        default:
            cerr << "Invalid ColorType " << colorType << '\n';
            break;
    }
    return vec;
}

#endif