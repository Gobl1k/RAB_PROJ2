#pragma once
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <array>

namespace bip = boost::interprocess;

constexpr const char* SHM_NAME = "HighPerfChannelSHM";
constexpr size_t SHM_SIZE = 64 * 1024;  // 64 КБ — с запасом

struct CompressedBlock {
    uint32_t block_id{0};
    uint32_t float_count{0};
    uint32_t compressed_size{0};
    bool     is_last{false};
    std::array<uint8_t, 236> data{};

    CompressedBlock() = default;
    CompressedBlock(const CompressedBlock&) = default;
    CompressedBlock& operator=(const CompressedBlock&) = default;
};

using ShmAllocator = bip::allocator<CompressedBlock, bip::managed_shared_memory::segment_manager>;
using ShmVector = bip::vector<CompressedBlock, ShmAllocator>;

struct SharedData {
    bip::interprocess_mutex      mutex;
    bip::interprocess_condition  cond_prod;
    bip::interprocess_condition  cond_cons;
    ShmVector                    queue;
    bool                         producer_finished{false};

    SharedData(const ShmAllocator& alloc) : queue(alloc) {}
};