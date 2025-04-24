cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(errie_leap)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Explicitly set compiler flag for C++23
add_compile_options(-std=c++2b)

# Add muParser
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/src/libs/muparser
    ${CMAKE_BINARY_DIR}/muparser  # Binary dir (build output location)
)

# Collect all C++ source files recursively
file(GLOB_RECURSE APP_SOURCES src/*.c src/*.cpp)

# Add all sources to the target
target_sources(app PRIVATE ${APP_SOURCES})

# Include directories for headers
target_include_directories(app PRIVATE src)

# Link muParser
target_link_libraries(app PRIVATE muparser)

# Print paths for debugging
# message(STATUS "muParser include path: ${CMAKE_CURRENT_SOURCE_DIR}/modules/muparser/include")