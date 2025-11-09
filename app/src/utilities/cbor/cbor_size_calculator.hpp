#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <zcbor_common.h>

namespace eerie_leap::utilities::cbor {

/**
 * CBOR Size Calculator
 *
 * Helper class to calculate encoded CBOR size without actually encoding.
 * Based on RFC 8949 encoding rules and zcbor conventions.
 */
class CborSizeCalc {
public:
    // ========================================================================
    // Basic Integer Encoding
    // ========================================================================

    /**
     * Calculate CBOR encoding size for unsigned integer
     */
    static size_t SizeOfUint(uint64_t value) {
        if (value <= 23) return 1;
        if (value <= 0xFF) return 2;
        if (value <= 0xFFFF) return 3;
        if (value <= 0xFFFFFFFF) return 5;
        return 9;
    }

    /**
     * Calculate CBOR encoding size for signed integer
     */
    static size_t SizeOfInt(int64_t value) {
        if (value >= 0) {
            return SizeOfUint(static_cast<uint64_t>(value));
        } else {
            // Negative integers encoded as -1 - value
            return SizeOfUint(static_cast<uint64_t>(-1 - value));
        }
    }

    /**
     * Calculate size for fixed-size integer (like uint .size 8)
     */
    static size_t SizeOfUintFixed(size_t bytes) {
        return 1 + bytes;  // 1 byte type + N bytes data
    }

    // ========================================================================
    // Strings and Byte Strings
    // ========================================================================

    /**
     * Calculate CBOR encoding overhead for length prefix
     */
    static size_t SizeOfLength(size_t length) {
        if (length <= 23) return 1;
        if (length <= 0xFF) return 2;
        if (length <= 0xFFFF) return 3;
        if (length <= 0xFFFFFFFF) return 5;
        return 9;
    }

    /**
     * Calculate CBOR encoding size for text string (tstr)
     */
    static size_t SizeOfTstr(const zcbor_string& str) {
        size_t len = str.len;
        return SizeOfLength(len) + len;
    }

    // ========================================================================
    // Arrays (Definite Length)
    // ========================================================================

    /**
     * Calculate CBOR array overhead for definite-length array
     */
    static size_t SizeOfArrayOverhead(size_t count) {
        return SizeOfLength(count);
    }

    /**
     * Start calculating definite-length array size
     * Usage:
     *   size_t size = CborSizeCalc::SizeOfArrayStart(3);
     *   size += CborSizeCalc::SizeOfUint(value1);
     *   size += CborSizeCalc::SizeOfTstr(str2);
     */
    static size_t SizeOfArrayStart(size_t count) {
        return SizeOfArrayOverhead(count);
    }

    /**
     * Calculate size for an array of uniform elements
     */
    static size_t SizeOfArrayUniform(size_t count, size_t elementSize) {
        return SizeOfArrayOverhead(count) + (count * elementSize);
    }

    /**
     * Calculate size for a vector of items with custom size calculator
     */
    template<typename T, typename SizeFunc>
    static size_t SizeOfArray(const std::vector<T>& items, SizeFunc sizeFunc) {
        size_t size = SizeOfArrayStart(items.size());
        for (const auto& item : items) {
            size += sizeFunc(item);
        }
        return size;
    }

    // ========================================================================
    // Arrays (Indefinite Length) - zcbor style
    // ========================================================================

    /**
     * Size for indefinite-length array markers (0x9f start + 0xff break)
     * This is what zcbor uses for [] syntax in CDDL
     */
    static size_t SizeOfIndefiniteArrayMarkers() {
        return 2;  // 0x9f (start) + 0xff (break)
    }

    /**
     * Start calculating indefinite-length array size
     * Returns just the markers; add element sizes separately
     */
    static size_t SizeOfIndefiniteArrayStart() {
        return SizeOfIndefiniteArrayMarkers();
    }

    /**
     * Calculate size for indefinite array with known elements
     */
    template<typename SizeFunc>
    static size_t SizeOfIndefiniteArray(size_t count, SizeFunc elementSizeFunc) {
        size_t size = SizeOfIndefiniteArrayMarkers();
        for (size_t i = 0; i < count; i++) {
            size += elementSizeFunc(i);
        }
        return size;
    }

    // ========================================================================
    // Maps
    // ========================================================================

    /**
     * Calculate CBOR map overhead for definite-length map
     * Note: count is number of key-value PAIRS, not total items
     */
    static size_t SizeOfMapOverhead(size_t pairCount) {
        return SizeOfLength(pairCount);
    }

    /**
     * Start calculating definite-length map size
     * Usage:
     *   size_t size = CborSizeCalc::SizeOfMapStart(2);
     *   size += CborSizeCalc::SizeOfTstr("key1") + CborSizeCalc::SizeOfUint(value1);
     */
    static size_t SizeOfMapStart(size_t pairCount) {
        return SizeOfMapOverhead(pairCount);
    }

    /**
     * Size for indefinite-length map markers (0xbf start + 0xff break)
     */
    static size_t SizeOfIndefiniteMapMarkers() {
        return 2;  // 0xbf (start) + 0xff (break)
    }

    /**
     * Start calculating indefinite-length map size
     */
    static size_t SizeOfIndefiniteMapStart() {
        return SizeOfIndefiniteMapMarkers();
    }

    // ========================================================================
    // Booleans and Special Values
    // ========================================================================

    /**
     * Calculate CBOR encoding size for boolean
     */
    static size_t SizeOfBool(bool value) {
        (void)value;  // Always 1 byte regardless of value
        return 1;
    }

    // ========================================================================
    // Floating Point
    // ========================================================================

    /**
     * Calculate CBOR encoding size for float (32-bit)
     */
    static size_t SizeOfFloat(float value) {
        (void)value;
        return 5;  // 1 byte type + 4 bytes data
    }

    /**
     * Calculate CBOR encoding size for double (64-bit)
     */
    static size_t SizeOfDouble(double value) {
        (void)value;
        return 9;  // 1 byte type + 8 bytes data
    }

    // ========================================================================
    // Optional Values
    // ========================================================================

    /**
     * Calculate size for optional field (returns 0 if not present)
     */
    template<typename T, typename SizeFunc>
    static size_t SizeOfOptional(const T* value, SizeFunc sizeFunc) {
        return value ? sizeFunc(*value) : 0;
    }

    /**
     * Calculate size for optional value based on boolean flag
     */
    template<typename T, typename SizeFunc>
    static size_t SizeOfOptional(bool hasValue, const T& value, SizeFunc sizeFunc) {
        return hasValue ? sizeFunc(value) : 0;
    }
};

} // namespace eerie_leap::utilities::cbor
