#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "cbor_size_calculator.hpp"

namespace eerie_leap::utilities::cbor {

/**
 * Helper class for building size calculations fluently
 */
class CborSizeBuilder {
private:
    size_t size_;

public:
    CborSizeBuilder() : size_(0) {}
    explicit CborSizeBuilder(size_t initialSize) : size_(initialSize) {}

    // Integer types
    CborSizeBuilder& AddUint(uint64_t value) {
        size_ += CborSizeCalc::SizeOfUint(value);
        return *this;
    }

    CborSizeBuilder& AddInt(int64_t value) {
        size_ += CborSizeCalc::SizeOfInt(value);
        return *this;
    }

    CborSizeBuilder& AddUintFixed(size_t bytes) {
        size_ += CborSizeCalc::SizeOfUintFixed(bytes);
        return *this;
    }

    // Strings
    CborSizeBuilder& AddTstr(const zcbor_string& str) {
        size_ += CborSizeCalc::SizeOfTstr(str);
        return *this;
    }

    // Special values
    CborSizeBuilder& AddBool(bool value) {
        size_ += CborSizeCalc::SizeOfBool(value);
        return *this;
    }

    CborSizeBuilder& AddFloat(float value) {
        size_ += CborSizeCalc::SizeOfFloat(value);
        return *this;
    }

    CborSizeBuilder& AddDouble(double value) {
        size_ += CborSizeCalc::SizeOfDouble(value);
        return *this;
    }

    // Definite-length arrays and maps
    CborSizeBuilder& AddArrayStart(size_t count) {
        size_ += CborSizeCalc::SizeOfArrayStart(count);
        return *this;
    }

    CborSizeBuilder& AddMapStart(size_t pairCount) {
        size_ += CborSizeCalc::SizeOfMapStart(pairCount);
        return *this;
    }

    // Indefinite-length arrays and maps (zcbor style)
    CborSizeBuilder& AddIndefiniteArrayStart() {
        size_ += CborSizeCalc::SizeOfIndefiniteArrayStart();
        return *this;
    }

    CborSizeBuilder& AddIndefiniteMapStart() {
        size_ += CborSizeCalc::SizeOfIndefiniteMapStart();
        return *this;
    }

    // Generic size addition
    CborSizeBuilder& AddSize(size_t additionalSize) {
        size_ += additionalSize;
        return *this;
    }

    // Optional values
    template<typename T, typename SizeFunc>
    CborSizeBuilder& AddOptional(const T* value, SizeFunc sizeFunc) {
        size_ += CborSizeCalc::SizeOfOptional(value, sizeFunc);
        return *this;
    }

    template<typename T, typename SizeFunc>
    CborSizeBuilder& AddOptional(bool hasValue, const T& value, SizeFunc sizeFunc) {
        size_ += CborSizeCalc::SizeOfOptional(hasValue, value, sizeFunc);
        return *this;
    }

    // Get result
    size_t Build() const {
        return size_;
    }

    size_t GetSize() const {
        return size_;
    }

    operator size_t() const {
        return size_;
    }

    // Reset
    void Reset() {
        size_ = 0;
    }
};

} // namespace eerie_leap::utilities::cbor
