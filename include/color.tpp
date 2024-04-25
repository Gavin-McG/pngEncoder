#ifndef COLOR_TPP
#define COLOR_TPP

#include "color.h"


template<typename T>
Color::Color(vector<T> vec) {
    switch (vec.size()) {
        case 0:
            *this = Color();
            break;
        case 1:
            *this = Color(vec[0]);
            break;
        case 2:
            *this = Color(vec[0],vec[1]);
            break;
        case 3:
            *this = Color(vec[0],vec[1],vec[2]);
            break;
        case 4:
            *this = Color(vec[0],vec[1],vec[2],vec[3]);
            break;
        default:
            *this = Color();
            break;
    }
}


template<typename T>
vector<T> Color::getbytes(ColorType colorType) const {
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