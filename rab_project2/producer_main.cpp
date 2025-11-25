#include <iostream>
#include "producer/producer.h" 

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: producer <input.bin>\n";
        return 1;
    }

    Producer p(argv[1]);
    p.run();
    return 0;
}