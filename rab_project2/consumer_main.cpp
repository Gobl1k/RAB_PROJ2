#include <iostream>
#include "consumer/consumer.h" 
int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: consumer <output.bin>\n";
        return 1;
    }

    Consumer c(argv[1]);
    c.run();
    return 0;
}