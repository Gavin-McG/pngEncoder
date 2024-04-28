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

#endif