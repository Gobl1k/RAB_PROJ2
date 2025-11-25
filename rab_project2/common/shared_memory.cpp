#pragma once
#include <cstdint>
#include <cstring>

constexpr size_t SHM_SIZE = 256;
constexpr size_t MAX_SLOTS = 8;
constexpr size_t PAYLOAD_SIZE = SHM_SIZE - 20;  // 236 байт

struct CompressedBlock {
    uint32_t block_id{0};
    uint32_t float_count{0};
    uint32_t compressed_size{0};
    bool     is_last{false};
    uint8_t  data[PAYLOAD_SIZE]{};

    void clear() { std::memset(this, 0, sizeof(*this)); }
};

struct SharedMemory {
    uint32_t write_idx{0};
    uint32_t read_idx{0};
    uint32_t ready{0};
    bool     producer_finished{false};
    CompressedBlock slots[MAX_SLOTS];
};

constexpr const char* SHM_NAME = "/high_perf_compression_channel";