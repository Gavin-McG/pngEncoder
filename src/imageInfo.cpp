#include "../include/imageInfo.h"



//default constructor
ImageInfo::ImageInfo() {
    width = 1;
    height = 1;
    colorType = DEFAULT_COLORTYPE;
    bitDepth = DEFAULT_BITDEPTH;

    filters = {FilterType::None};
    ref = {{DEFAULT_COLOR}};
}

//width+height constructors
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


ImageInfo& ImageInfo::operator=(const ImageInfo &other) {
    width = other.width;
    height = other.height;
    colorType = other.colorType;
    bitDepth = other.bitDepth;

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

    return *this;
}


void ImageInfo::setFilters(FilterType type) {
    for (size_t i=0; i<filters.size(); ++i) {
        filters[i] = type;
    }
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