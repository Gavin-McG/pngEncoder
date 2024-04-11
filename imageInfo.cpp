#include "imageInfo.h"

ImageInfo::ImageInfo(uint32_t width, uint32_t height, ColorType colorType, BitDepth bitDepth, const Color &color) {
    this->width = width;
    this->height = height;
    this->colorType = colorType;
    this->bitDepth = bitDepth;

    filters.resize(height);
    ref.resize(height);
    for (size_t i=0; i<ref.size(); ++i) {
        ref[i].resize(width, color);
    }
}
ImageInfo::ImageInfo(uint32_t width, uint32_t height, BitDepth bitdepth, const Color &color) : 
    ImageInfo(width, height, DEFAULT_COLORTYPE, bitdepth, color) {}
ImageInfo::ImageInfo(uint32_t width, uint32_t height, ColorType colorType, const Color &color) : 
    ImageInfo(width, height, colorType, DEFAULT_BITDEPTH, color) {}
ImageInfo::ImageInfo(uint32_t width, uint32_t height, const Color &color) : 
    ImageInfo(width, height, DEFAULT_COLORTYPE, DEFAULT_BITDEPTH, color) {}


void ImageInfo::setFilters(FilterType type) {
    for (size_t i=0; i<filters.size(); ++i) {
        filters[i] = type;
    }
}

void ImageInfo::calculateBestFilters() {

}

size_t ImageInfo::getScanlineSize() const {
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


vector<uint8_t> ImageInfo::getDatastream() const {
    vector<uint8_t> vec;
    vec.reserve(getScanlineSize()*height);

    switch (bitDepth) {
        case BitDepth::One:
            
            break;
        case BitDepth::Two:
            
            break;
        case BitDepth::Four:
            
            break;
        case BitDepth::Eight:
            for (size_t i=0; i<height; ++i) {
                vector<uint8_t> scanline = getScanline<uint8_t>(i,filters[i]);
                for (size_t j=0; j<scanline.size(); ++j) {
                    vec.push_back(scanline[j]);
                }
            }
            break;
        case BitDepth::Sixteen:
            for (size_t i=0; i<height; ++i) {
                vector<uint8_t> scanline = getScanline<uint16_t>(i,filters[i]);
                for (size_t j=0; j<scanline.size(); ++j) {
                    vec.push_back(scanline[j]);
                }
            }
            break;
        default: break;
    }
    
    return vec;
}

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    int16_t p = static_cast<int16_t>(a) + static_cast<int16_t>(b) - static_cast<int16_t>(c);
    int16_t pa = abs(p - static_cast<int16_t>(a));
    int16_t pb = abs(p - static_cast<int16_t>(b));
    int16_t pc = abs(p - static_cast<int16_t>(c));
    if (pa <= pb && pa <= pc) return static_cast<uint8_t>(a);
    else if (pb <= pc) return static_cast<uint8_t>(b);
    return static_cast<uint8_t>(c);
}