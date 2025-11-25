cmake_minimum_required(VERSION 3.14)
project(HighPerfChannel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS interprocess)

add_executable(producer producer_main.cpp producer/producer.cpp compressor/compressor.cpp)
add_executable(consumer consumer_main.cpp consumer/consumer.cpp compressor/compressor.cpp)

target_link_libraries(producer PRIVATE Boost::interprocess)
target_link_libraries(consumer PRIVATE Boost::interprocess)

target_include_directories(producer PRIVATE . common compressor)
target_include_directories(consumer PRIVATE . common compressor)