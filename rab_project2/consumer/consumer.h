#pragma once
#include <string>

class Consumer {
public:
    explicit Consumer(const std::string& output_path);
    void run();

private:
    std::string output_file;
};