#pragma once

#include <Arduino.h>
#include "config.hpp"

template <typename T>
class DynamicArray {
    public:
        DynamicArray(size_t minSize = ARRAY_MIN_SIZE);
        const T & operator[](size_t index) const;
        T & operator[](size_t index);
        void append(const T & value);
        T & pop(size_t index);
        size_t size() const;
        size_t len() const;

    private:
        T * _values;
        size_t _maxSize;
        size_t _size;
        size_t _minSize;
};
