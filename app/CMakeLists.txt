cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(errie_leap)

add_subdirectory(src/domain)
add_subdirectory(src/threads)

include_directories(src/)
include_directories(src/domain)
include_directories(src/threads)

target_sources(app PRIVATE src/main.cpp)
