#include "producer.h"
#include "../common/shared_memory.h"
#include "../compressor/compressor.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>
#include <fstream>
#include <mutex>

namespace bip = boost::interprocess;

Producer::Producer(const std::string& path) : input_file(path) {}

void Producer::run()
{
    bip::shared_memory_object::remove(SHM_NAME);

    bip::managed_shared_memory segment(bip::create_only, SHM_NAME, SHM_SIZE);
    ShmAllocator alloc(segment.get_segment_manager());
    SharedData* data = segment.construct<SharedData>("Data")(alloc);

    std::ifstream in(input_file, std::ios::binary);
    if (!in) {
        std::cerr << "Producer: cannot open input file\n";
        return;
    }

    std::vector<float> buffer(14000);
    uint32_t block_id = 0;
    double start = Compressor::now();

    while (in) {
        in.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(float));
        size_t count = in.gcount() / sizeof(float);
        if (count == 0) break;

        CompressedBlock block;
        block.block_id = block_id++;
        block.float_count = static_cast<uint32_t>(count);
        block.compressed_size = Compressor::compress(buffer.data(), count, block.data.data(), block.data.size());
        block.is_last = (count < buffer.size());

        {
            std::scoped_lock<bip::interprocess_mutex> lock(data->mutex);
            data->queue.push_back(block);
        }
        data->cond_cons.notify_one();
    }

    {
        std::scoped_lock<bip::interprocess_mutex> lock(data->mutex);
        data->producer_finished = true;
    }
    data->cond_cons.notify_one();

    double end = Compressor::now();
    std::cout << "Producer finished in " << (end - start) << "s, sent " << block_id << " blocks\n";
}