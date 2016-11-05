cmake_minimum_required(VERSION 3.6)
project(parallel_omp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 -fopenmp ")
include_directories("/usr/include/python3.5m")
set(SOURCE_FILES main.cpp)

add_executable(parallel_omp ${SOURCE_FILES})
target_link_libraries(parallel_omp python3.5m)
