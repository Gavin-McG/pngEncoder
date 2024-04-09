#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include "utilities.h"

using namespace std;

class Color {
    public:
    float r,g,b,a;

    Color();
    Color(float v);
    Color(float v, float a);
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);

    Color(const Color &c);

    Color operator+(const Color &other);
    Color operator-(const Color &other);
    Color operator*(const Color &other);

    float getGreyscale();

    template<typename T>
    vector<T> getbytes(ColorType colorType);
};

Color operator*(const float f, const Color &color);
Color operator*(const Color &color, const float f);

#include "color.tpp"

#endif