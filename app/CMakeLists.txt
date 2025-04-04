cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(errie_leap)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure C++ support is enabled
set(CONFIG_CPLUSPLUS y)

# Explicitly set compiler flag for C++23
add_compile_options(-std=c++2b)

add_subdirectory(src/domain)

include_directories(src/)
include_directories(src/domain)
include_directories(src/domain/controllers)

target_sources(app PRIVATE src/main.cpp)
