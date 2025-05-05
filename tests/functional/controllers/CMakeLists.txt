cmake_minimum_required(VERSION 3.20.0)

# Use Zephyr's build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_guid_generator)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to muparser source and header
set(MUPARSER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/libs/muparser/src)
set(MUPARSER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/libs/muparser/include)
file(GLOB_RECURSE MUPARSER_SOURCES ${MUPARSER_SRC_DIR}/*.cpp)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src
  ${MUPARSER_INCLUDE_DIR}
)

set(APP_SRC_CONFIGURATION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/configuration)
file(GLOB_RECURSE APP_SRC_CONFIGURATION
  ${APP_SRC_CONFIGURATION_DIR}/*.c
  ${APP_SRC_CONFIGURATION_DIR}/*.cpp
)

set(APP_SRC_CONTROLLERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/controllers)
file(GLOB_RECURSE APP_SRC_CONTROLLERS
  ${APP_SRC_CONTROLLERS_DIR}/*.c
  ${APP_SRC_CONTROLLERS_DIR}/*.cpp
)

set(APP_SRC_FS_DOMAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/domain/fs_domain)
file(GLOB_RECURSE APP_SRC_FS_DOMAIN
  ${APP_SRC_FS_DOMAIN_DIR}/*.c
  ${APP_SRC_FS_DOMAIN_DIR}/*.cpp
)

set(APP_SRC_SENSOR_DOMAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/domain/sensor_domain)
file(GLOB_RECURSE APP_SRC_SENSOR_DOMAIN_MODELS
  ${APP_SRC_SENSOR_DOMAIN_DIR}/models/*.c
  ${APP_SRC_SENSOR_DOMAIN_DIR}/models/*.cpp
)
file(GLOB_RECURSE APP_SRC_SENSOR_DOMAIN_UTILITIES
  ${APP_SRC_SENSOR_DOMAIN_DIR}/utilities/*.c
  ${APP_SRC_SENSOR_DOMAIN_DIR}/utilities/*.cpp
)

# Collect all source files for the test
set(APP_SRC_UTILITIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/utilities)
file(GLOB_RECURSE APP_SRC_UTILITIES
  ${APP_SRC_UTILITIES_DIR}/*.c
  ${APP_SRC_UTILITIES_DIR}/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_CONFIGURATION}
  ${APP_SRC_CONTROLLERS}
  ${APP_SRC_FS_DOMAIN}
  ${APP_SRC_SENSOR_DOMAIN_MODELS}
  ${APP_SRC_SENSOR_DOMAIN_UTILITIES}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
  ${MUPARSER_SOURCES}
)
