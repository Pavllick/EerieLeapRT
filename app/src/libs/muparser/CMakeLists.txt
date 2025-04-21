# modules/muparser/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Disable muParser's built-in CMake settings (to avoid conflicts with Zephyr)
set(MUPARSER_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(MUPARSER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MUPARSER_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# Define muParser as a static library
add_library(muparser STATIC
    src/muParser.cpp
    src/muParserBase.cpp
    src/muParserBytecode.cpp
    src/muParserCallback.cpp
    src/muParserError.cpp
    src/muParserInt.cpp
    src/muParserTokenReader.cpp
)

# Make headers available to Zephyr
target_include_directories(muparser PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Zephyr-compatible compiler flags (disable warnings treated as errors)
target_compile_options(muparser PRIVATE 
    -Wno-unused-parameter
    -Wno-maybe-uninitialized
)

# Force C++17 (muParser's minimum required for exceptions)
target_compile_features(muparser PRIVATE cxx_std_17)