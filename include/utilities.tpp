#ifndef UTILITIES_TPP
#define UTILITIES_TPP

#include "utilities.h"

template<typename T>
T packInt(char* data) {
    size_t length = sizeof(T);
    T result = 0;
    for (size_t i=0; i<length; i++) {
        result = (result << 8) + static_cast<uint8_t>(data[i]);
    }
    return result;
}

#endif