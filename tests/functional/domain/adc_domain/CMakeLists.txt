cmake_minimum_required(VERSION 3.20.0)

# Use Zephyr's build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_guid_generator)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/src)

include_directories(${APP_SRC_DIR})

# Path to boost include directory
set(BOOST_INCLUDE_DIR ${APP_SRC_DIR}/../libs/boost)
include_directories(${BOOST_INCLUDE_DIR})

add_subdirectory(
  ${APP_SRC_DIR}/../libs/eerie_memory
  ${CMAKE_CURRENT_BINARY_DIR}/eerie_memory)

target_link_libraries(app PRIVATE
    eerie_memory
)

set(APP_SRC_SUBSYS_DT_DIR ${APP_SRC_DIR}/subsys/device_tree)
file(GLOB_RECURSE APP_SRC_SUBSYS_DT
  ${APP_SRC_SUBSYS_DIR}/dt_logger.cpp
  ${APP_SRC_SUBSYS_DT_DIR}/dt_adc.cpp
)

set(APP_SRC_ADC_SUBSYS_DIR ${APP_SRC_DIR}/subsys/adc)
file(GLOB_RECURSE APP_SRC_ADC_SUBSYS
  ${APP_SRC_ADC_SUBSYS_DIR}/*.cpp
)

set(APP_SRC_UTILITIES_DIR ${APP_SRC_DIR}/utilities/memory)
file(GLOB_RECURSE APP_SRC_UTILITIES
  ${APP_SRC_UTILITIES_DIR}/*.c
  ${APP_SRC_UTILITIES_DIR}/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_SUBSYS_DT}
  ${APP_SRC_ADC_SUBSYS}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
)
