cmake_minimum_required(VERSION 3.20)

# Bootstrap and build Boost
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/../modules/boost/b2")
    message(STATUS "Bootstrapping Boost...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/bootstrap.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE BOOTSTRAP_RESULT
    )
    if(NOT BOOTSTRAP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to bootstrap Boost")
    endif()

    message(STATUS "Building Boost headers and tools...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/b2 headers tools/bcp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE B2_HEADERS_RESULT
    )
    if(NOT B2_HEADERS_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to build Boost headers and tools")
    endif()

    message(STATUS "Extracting Boost.Spirit.X3 and Boost.Fusion headers...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/dist/bin/bcp boost/spirit boost/fusion ${CMAKE_SOURCE_DIR}/include/boost
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE BCP_RESULT
    )
    if(NOT BCP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to extract Boost.Spirit.X3 and Boost.Fusion headers")
    endif()
endif()

# Apply patches before build
function(apply_patches patch_dir target_dir)
    file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "${patch_dir}/*.patch")
    foreach(file ${files})
        execute_process(
            COMMAND git apply --reverse --check ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
            WORKING_DIRECTORY ${target_dir}
            RESULT_VARIABLE patch_already_applied
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(patch_already_applied EQUAL 0)
            message("Patch already applied: ${file}")
        else()
            message("Applying patch: ${file}")
            execute_process(
                COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
                WORKING_DIRECTORY ${target_dir}
                RESULT_VARIABLE patch_apply_result
            )
            if(NOT patch_apply_result EQUAL 0)
                message(FATAL_ERROR "Failed to apply patch: ${file}")
            endif()
        endif()
    endforeach()
endfunction()

apply_patches("patches/zephyr" $ENV{ZEPHYR_BASE})

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

# Include headers
target_include_directories(app PRIVATE
  include/boost
  src
  src/libs/muparser/include
  src/libs/dbcppp/include
)

# Add sources to the app target
target_sources(app PRIVATE ${APP_SOURCES})
