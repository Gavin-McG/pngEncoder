#include "imageInfo.h"






ImageInfo::ImageInfo(uint32_t width, uint32_t height, ColorType colorType, BitDepth bitDepth, const Color &color) {
    if (colorType != ColorType::Grey && (bitDepth < 8)) {
        cerr << "Inavlid colorType+bitdepth combination" << endl;
    }

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



ImageInfo::ImageInfo(const ImageInfo &other) : 
    width(other.width), height(other.height), bitDepth(other.bitDepth), colorType(other.colorType)
{
    //copy filters
    filters = vector<FilterType>(other.filters.begin(),other.filters.end());

    //copy image
    ref = vector<vector<Color>>(other.height);
    for (size_t i=0;i<other.height;++i) {
        ref[i].reserve(other.width);
        for (size_t j=0;j<other.width;++j) {
            ref[i].push_back(other.ref[i][j]);
        }
    }
}


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






void ImageInfo::drawPixel(uint32_t x, uint32_t y, const Color &color) {
    if (x<width && y<height) ref[y][x] = color;
}

void ImageInfo::drawRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color &color) {
    for (uint32_t i=x; i<x+width && i<this->width; ++i) {
        for (uint32_t j=y; j<y+height && j<this->height; ++j) {
            ref[j][i] = color;
        }
    }
}





Color ImageInfo::getPixel(uint32_t x, uint32_t y) {
    if (x<width && y<height) return ref[y][x];
    return Color(0,0);
}








void ImageInfo::printPng(ostream &os, DeflateType deflateType) const {
    printSig(os);
    printIDHR(os);
    printIDAT(os, deflateType);
    printIEND(os);
}



void ImageInfo::printSig(ostream &os) const {
    os << "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a";
}

void ImageInfo::printIDHR(ostream &os) const {
    if (!crcGenerated) generateCRC(crcTable);

    //length of chunk
    printInt(os, uint32_t(13)); //length

    //add name and datastream to ss
    stringstream ss;
    ss << "IHDR";
    printInt(ss, width); //width
    printInt(ss, height); //height
    printInt(ss, bitDepth); //bit depth
    printInt(ss, static_cast<uint8_t>(colorType)); //color type
    printInt(ss, uint8_t(0)); //compression method
    printInt(ss, uint8_t(0)); //filter method
    printInt(ss, uint8_t(0)); //interlacing

    //add checksum of length and datastream to ss
    printInt(ss,getCRC32(ss, crcTable));

    //output ss to ostream
    os << ss.str();
}

void ImageInfo::printIDAT(ostream &os, DeflateType deflateType) const {
    if (!crcGenerated) generateCRC(crcTable);

    //get and compress data
    vector<uint8_t> data = getDatastream();
    vector<uint8_t> compressed = deflate(data, deflateType);
    
    //get chunk name and data to compute crc32
    stringstream ss;
    ss << "IDAT";
    for (uint8_t i : compressed) {
        ss << i;
    }

    //clear compressed to save memory
    uint32_t length = compressed.size();
    compressed = vector<uint8_t>();

    //compute crc32
    uint32_t crc = getCRC32(ss,crcTable);

    //chunk data length
    printInt(os,length);
    //name and data
    os << ss.str();
    //crc32
    printInt(os,crc);
}

void ImageInfo::printIEND(ostream &os) const {
    if (!crcGenerated) generateCRC(crcTable);

    printInt(os, static_cast<uint32_t>(0));
    os << "IEND";
    printInt(os,getCRC32("IEND", crcTable));
}









ImageInfo ImageInfo::filter(const ImageInfo &kernel, bool normalize) const {
    //get total weight of kernel
    Color factor;
    if (normalize) {
        Color totalWeight(0,0,0,0);
        for (size_t i=0;i<kernel.height;++i) {
            for (size_t j=0;j<kernel.width;++j) {
                totalWeight += kernel.ref[i][j];
            }
        }
        if (totalWeight.r==0 || totalWeight.g==0 || totalWeight.b==0 || totalWeight.a==0) {
            normalize = false;
        }else{
            factor = 1/totalWeight;
        }
    }


    ImageInfo newImage(*this);

    for (size_t i=0;i<height;++i) {
        for (size_t j=0;j<width;++j) {
            Color colorSum(0,0,0,0);
            for (size_t ki=0;ki<kernel.height;++ki) {
                int y = static_cast<int>(i+ki) - kernel.height/2;
                if (y<0 || y>=static_cast<int>(height)) continue;

                for (size_t kj=0;kj<kernel.width;++kj) {
                    int x = static_cast<int>(j+kj) - kernel.width/2;
                    if (x<0 || x>=static_cast<int>(width)) continue;

                    colorSum += kernel.ref[ki][kj]*ref[y][x];
                }
            }
            if (normalize) colorSum*=factor;

            newImage.ref[i][j] = colorSum;
        }
    }

    return newImage;
}



ImageInfo getGaussian(size_t N, float sigma) {
    ImageInfo gaussian(N,N,Color(1));

    //set initial values
    float totalWeight = 0;
    for (size_t i=0;i<N;++i) {
        for (size_t j=0;j<N;++j) {
            int y = static_cast<int>(i) - static_cast<int>(N/2);
            int x = static_cast<int>(j) - static_cast<int>(N/2);
            float r = static_cast<float>(x*x+y*y);

            float v = pow(M_E,-r/(2*sigma*sigma));

            gaussian.ref[i][j] *= v;
            totalWeight += v;
        }
    }

    //normalize values
    for (size_t i=0;i<N;++i) {
        for (size_t j=0;j<N;++j) {
            gaussian.ref[i][j] /= totalWeight;
        }
    }

    return gaussian;
}