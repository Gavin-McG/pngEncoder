#include "../include/color.h"

//constructors
Color::Color() : r(0), g(0), b(0), a(1) {}

Color::Color(float v) : r(v), g(v), b(v), a(1) {}
Color::Color(float v, float a) : r(v), g(v), b(v), a(a) {}
Color::Color(float r, float g, float b) : r(r), g(g), b(b), a(1) {}
Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

//copy constructor
Color::Color(const Color &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}


//assignment operator
Color& Color::operator=(const Color& other) {
    if (this != &other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }
    return *this;
}


//operators
Color Color::operator+(const Color &other) const {
    return Color(r+other.r, g+other.g, b+other.b, a+other.a);
}

Color Color::operator-(const Color &other) const {
    return Color(r-other.r, g-other.g, b-other.b, a-other.a);
}

Color Color::operator*(const Color &other) const {
    return Color(r*other.r, g*other.g, b*other.b, a*other.a);
}

Color Color::operator/(const Color &other) const {
    return Color(r/other.r, g/other.g, b/other.b, a/other.a);
}





Color& Color::operator+=(const Color &other) {
    r += other.r;
    g += other.g;
    b += other.b;
    a += other.a;
    return *this;
}

Color& Color::operator-=(const Color &other) {
    r -= other.r;
    g -= other.g;
    b -= other.b;
    a -= other.a;
    return *this;
}

Color& Color::operator*=(const Color &other) {
    r *= other.r;
    g *= other.g;
    b *= other.b;
    a *= other.a;
    return *this;
}

Color& Color::operator/=(const Color &other) {
    r /= other.r;
    g /= other.g;
    b /= other.b;
    a /= other.a;
    return *this;
}





float Color::getGreyscale() const {
    return 0.299*r + 0.587*g + 0.114*b;
}





Color operator*(const float f, const Color &color) {
    return Color(color.r*f, color.g*f, color.b*f, color.a*f);
}

Color operator*(const Color &color, const float f) {
    return Color(color.r*f, color.g*f, color.b*f, color.a*f);
}


Color operator/(const float f, const Color &color) {
    return Color(f/color.r, f/color.g, f/color.b, f/color.a);
}

Color operator/(const Color &color, const float f) {
    return Color(color.r/f, color.g/f, color.b/f, color.a/f);
}



ostream& operator<<(ostream &os, const Color &color) {
    os << '(' << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ')';
    return os;
}


Color colorMin(const Color &c1, const Color &c2) {
    return Color(min(c1.r,c2.r),min(c1.g,c2.g),min(c1.b,c2.b),min(c1.a,c2.a));
}
Color colorMax(const Color &c1, const Color &c2) {
    return Color(max(c1.r,c2.r),max(c1.g,c2.g),max(c1.b,c2.b),max(c1.a,c2.a));
}


Color abs(const Color &c) {
    return Color(abs(c.r),abs(c.g),abs(c.b),abs(c.a));
}