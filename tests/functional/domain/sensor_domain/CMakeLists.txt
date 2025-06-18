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

# Path to muparser source and header
set(MUPARSER_SRC_DIR ${APP_SRC_DIR}/libs/muparser/src)
set(MUPARSER_INCLUDE_DIR ${APP_SRC_DIR}/libs/muparser/include)
file(GLOB_RECURSE MUPARSER_SOURCES ${MUPARSER_SRC_DIR}/*.cpp)

include_directories(${MUPARSER_INCLUDE_DIR})

set(APP_SRC_CONFIGURATION_DIR ${APP_SRC_DIR}/configuration)
file(GLOB_RECURSE APP_SRC_CONFIGURATION
  ${APP_SRC_CONFIGURATION_DIR}/*.c
  ${APP_SRC_CONFIGURATION_DIR}/*.cpp
)

set(APP_SRC_CONTROLLERS_DIR ${APP_SRC_DIR}/controllers)
file(GLOB_RECURSE APP_SRC_CONTROLLERS
  ${APP_SRC_CONTROLLERS_DIR}/*.c
  ${APP_SRC_CONTROLLERS_DIR}/*.cpp
)

set(APP_SRC_DOMAIN_DIR ${APP_SRC_DIR}/domain)
file(GLOB_RECURSE APP_SRC_DOMAIN
  ${APP_SRC_DOMAIN_DIR}/hardware/adc_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/hardware/adc_domain/*.cpp
  ${APP_SRC_DOMAIN_DIR}/hardware/gpio_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/hardware/gpio_domain/*.cpp
  ${APP_SRC_DOMAIN_DIR}/fs_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/fs_domain/*.cpp
  ${APP_SRC_DOMAIN_DIR}/sensor_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/sensor_domain/*.cpp
  ${APP_SRC_DOMAIN_DIR}/system_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/system_domain/*.cpp
)

# Collect all source files for the test
set(APP_SRC_UTILITIES_DIR ${APP_SRC_DIR}/utilities)
file(GLOB_RECURSE APP_SRC_UTILITIES
  ${APP_SRC_UTILITIES_DIR}/*.c
  ${APP_SRC_UTILITIES_DIR}/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_CONFIGURATION}
  ${APP_SRC_CONTROLLERS}
  ${APP_SRC_DOMAIN}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
  ${MUPARSER_SOURCES}
)
