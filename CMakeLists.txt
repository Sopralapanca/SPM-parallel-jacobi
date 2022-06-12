cmake_minimum_required(VERSION 3.21)
project(JacobiSPM)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-O3)

add_executable(JacobiSPM sequential.cpp parallel2.cpp ./utils/utimer.cpp)
