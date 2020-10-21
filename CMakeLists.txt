cmake_minimum_required(VERSION 3.11)
project(PowerPC_HLS)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)
add_executable(PowerPC_HLS
        src/main.cpp
        src/decode_utils.hpp
        src/registers.hpp
        src/ppc_types.h
        src/fixed_point_utils.hpp
        src/fixed_point_processor.hpp
        src/instruction_decode.hpp
        src/test_bench_utils.hpp
        src/fixed_point_utils.cpp
        src/fixed_point_processor.cpp
        src/instruction_decode.cpp
        src/test_bench_utils.cpp)