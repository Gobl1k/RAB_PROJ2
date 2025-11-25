#include "consumer.h"
#include "../common/shared_memory.h"
#include "../compressor/compressor.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>
#include <fstream>
#include <mutex>

namespace bip = boost::interprocess;

Consumer::Consumer(const std::string& path) : output_file(path) {}

void Consumer::run()
{
    std::ofstream out(output_file, std::ios::binary);
    if (!out) {
        std::cerr << "Consumer: cannot create output file\n";
        return;
    }

    bip::managed_shared_memory segment(bip::open_only, SHM_NAME);
    std::pair<SharedData*, std::size_t> ret = segment.find<SharedData>("Data");
    SharedData* data = ret.first;

    std::vector<float> buffer(20000);
    double start = Compressor::now();

    while (true) {
        CompressedBlock block;
        {
            std::unique_lock<bip::interprocess_mutex> lock(data->mutex);
            data->cond_cons.wait(lock, [&]() {
                return !data->queue.empty() || data->producer_finished;
            });

            if (data->queue.empty() && data->producer_finished) break;

            block = std::move(data->queue.front());
            data->queue.erase(data->queue.begin());
        }
        data->cond_prod.notify_one();

        size_t decoded = Compressor::decompress(block.data.data(), block.compressed_size, buffer.data(), buffer.size());
        out.write(reinterpret_cast<const char*>(buffer.data()), decoded * sizeof(float));

        if (block.is_last) break;
    }

    double end = Compressor::now();
    std::cout << "Consumer finished in " << (end - start) << "s\n";

    bip::shared_memory_object::remove(SHM_NAME);
}