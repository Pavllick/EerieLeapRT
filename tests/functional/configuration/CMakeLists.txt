cmake_minimum_required(VERSION 3.20.0)

set(EL_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/eerie_leap_common)

set(EXTRA_ZEPHYR_MODULES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/eerie_leap_lua"
    "${EL_COMMON_DIR}")

# Use Zephyr's build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_guid_generator)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src)
include_directories(${APP_SRC_DIR})

set(APP_SRC_CONFIGURATION_DIR ${APP_SRC_DIR}/configuration)
file(GLOB_RECURSE APP_SRC_CONFIGURATION
  ${APP_SRC_CONFIGURATION_DIR}/*.c
  ${APP_SRC_CONFIGURATION_DIR}/*.cpp
  ${EL_COMMON_DIR}/src/configuration/cbor/traits/system_config_trait.cpp
  ${EL_COMMON_DIR}/src/configuration/cbor/cbor_system_config/*.c
  ${EL_COMMON_DIR}/src/configuration/cbor/traits/adc_config_trait.cpp
  ${EL_COMMON_DIR}/src/configuration/cbor/cbor_adc_config/*.cpp
  ${EL_COMMON_DIR}/src/configuration/cbor/traits/sensors_config_trait.cpp
  ${EL_COMMON_DIR}/src/configuration/cbor/cbor_sensors_config/*.cpp
)

set(APP_SRC_SUBSYS_DIR ${APP_SRC_DIR}/subsys)
file(GLOB_RECURSE APP_SRC_SUBSYS
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_fs.cpp
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_logger.cpp
  ${APP_SRC_SUBSYS_DIR}/adc/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_CONFIGURATION}
  ${APP_SRC_SUBSYS}
  ${TEST_SOURCES}
)
