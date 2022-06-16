cmake_minimum_required(VERSION 3.21)
project(JacobiSPM)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-O3)

add_executable(JacobiSPM parallel_row_cyclic_barrier.cpp ./utils/utimer.cpp utils/utility.h utils/utility.cpp)
