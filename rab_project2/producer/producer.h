#pragma once
#include <string>

class Producer {
public:
    explicit Producer(const std::string& input_path);
    void run();

private:
    std::string input_file;
};