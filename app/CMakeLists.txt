cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(errie_leap)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++23 globally (Zephyr-compatible way)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Process nested HTTP domain CMake file
add_subdirectory(src/domain/http_domain)

# Collect all source files
file(GLOB_RECURSE APP_SOURCES
  src/*.cpp
  src/*.c
)

# Add sources to the app target
target_sources(app PRIVATE ${APP_SOURCES})

# Include headers
target_include_directories(app PRIVATE
  src
  src/libs/muparser/include
)
