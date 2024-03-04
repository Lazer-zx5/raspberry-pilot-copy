#include "array.hpp"
#include <Arduino.h>

template <typename T> DynamicArray<T>::DynamicArray(size_t minSize = ARRAY_MIN_SIZE) {
    _values = (T *)malloc(minSize)
    if (_values == NULL) {

    }

}
