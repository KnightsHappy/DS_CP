cmake_minimum_required(VERSION 3.10)
project(GenomicSearchEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable aggressive optimizations for benchmarking
set(CMAKE_CXX_FLAGS "-O3")
if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /W3")
endif()

add_executable(GenomicSearchEngine main.cpp GenomeIndexer.cpp)
