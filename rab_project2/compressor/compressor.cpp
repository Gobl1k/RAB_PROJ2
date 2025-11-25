#include "compressor.h"
#include <chrono>
#include <algorithm>
#include <cstring>

size_t Compressor::compress(const float* src, size_t count, uint8_t* dst, size_t dst_capacity)
{
    if (count == 0) return 0;

    float min_val = src[0], max_val = src[0];
    for (size_t i = 1; i < count; ++i) {
        min_val = std::min(min_val, src[i]);
        max_val = std::max(max_val, src[i]);
    }
    float range = (max_val - min_val < 1e-8f) ? 1.0f : (max_val - min_val);

    std::memcpy(dst, &min_val, 4);
    std::memcpy(dst + 4, &max_val, 4);
    uint16_t cnt16 = static_cast<uint16_t>(std::min(count, size_t(65535)));
    std::memcpy(dst + 8, &cnt16, 2);

    size_t offset = 10;
    size_t to_write = std::min(cnt16, static_cast<uint16_t>(dst_capacity - offset));

    for (size_t i = 0; i < to_write; ++i) {
        float norm = (src[i] - min_val) / range;
        dst[offset++] = static_cast<uint8_t>(norm * 255.0f + 0.5f);
    }
    return offset;
}

size_t Compressor::decompress(const uint8_t* src, size_t src_size, float* dst, size_t dst_capacity)
{
    if (src_size < 10) return 0;

    float min_val, max_val;
    uint16_t count;
    std::memcpy(&min_val, src, 4);
    std::memcpy(&max_val, src + 4, 4);
    std::memcpy(&count, src + 8, 2);

    float range = (max_val - min_val < 1e-8f) ? 1.0f : (max_val - min_val);
    size_t to_read = std::min(size_t(count), dst_capacity);

    for (size_t i = 0; i < to_read; ++i) {
        float norm = src[10 + i] / 255.0f;
        dst[i] = min_val + norm * range;
    }
    return to_read;
}

double Compressor::now()
{
    return std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}