#pragma once
#include <cstddef>
#include <cstdint>

class Compressor {
public:
    static size_t compress(const float* src, size_t count, uint8_t* dst, size_t dst_capacity);
    static size_t decompress(const uint8_t* src, size_t src_size, float* dst, size_t dst_capacity);
    static double now();
};