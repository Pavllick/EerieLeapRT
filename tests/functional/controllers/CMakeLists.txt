cmake_minimum_required(VERSION 3.20.0)

# Add the Lua Zephyr module
set(EXTRA_ZEPHYR_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/lua_zephyr")

# Use Zephyr's build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_guid_generator)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-DCMAKE_EXPORT_COMPILE_COMMANDS=1)

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src)
include_directories(${APP_SRC_DIR})

# Path to boost include directory
set(BOOST_INCLUDE_DIR ${APP_SRC_DIR}/../libs/boost)
include_directories(${BOOST_INCLUDE_DIR})

add_definitions(-DBOOST_SYSTEM_DISABLE_THREADS)

add_subdirectory(
  ${APP_SRC_DIR}/../libs/muparser
  ${CMAKE_CURRENT_BINARY_DIR}/muparser)
add_subdirectory(
  ${APP_SRC_DIR}/../libs/nameof
  ${CMAKE_CURRENT_BINARY_DIR}/nameof)

target_link_libraries(app PRIVATE
    muparser
    nameof
)

set(APP_SRC_CONFIGURATION_DIR ${APP_SRC_DIR}/configuration)
file(GLOB_RECURSE APP_SRC_CONFIGURATION
  ${APP_SRC_CONFIGURATION_DIR}/*.c
  ${APP_SRC_CONFIGURATION_DIR}/*.cpp
)

set(APP_SRC_SUBSYS_DT_DIR ${APP_SRC_DIR}/subsys/device_tree)
file(GLOB_RECURSE APP_SRC_SUBSYS_DT
  ${APP_SRC_SUBSYS_DT_DIR}/dt_logger.cpp
  ${APP_SRC_SUBSYS_DT_DIR}/dt_fs.cpp
)

set(APP_SRC_SYSTEM_DOMAIN_DIR ${APP_SRC_DIR}/domain/system_domain)
file(GLOB_RECURSE APP_SRC_SYSTEM_DOMAIN
  ${APP_SRC_SYSTEM_DOMAIN_DIR}/*.c
  ${APP_SRC_SYSTEM_DOMAIN_DIR}/*.cpp
)

set(APP_SRC_ADC_SUBSYS_DIR ${APP_SRC_DIR}/subsys/adc)
file(GLOB_RECURSE APP_SRC_ADC_SUBSYS
  ${APP_SRC_ADC_SUBSYS_DIR}/*.c
  ${APP_SRC_ADC_SUBSYS_DIR}/*.cpp
)

set(APP_SRC_SUBSYS_DIR ${APP_SRC_DIR}/subsys)
file(GLOB_RECURSE APP_SRC_SUBSYS
  ${APP_SRC_SUBSYS_DIR}/adc/*.c
  ${APP_SRC_SUBSYS_DIR}/adc/*.cpp
  ${APP_SRC_SUBSYS_DIR}/gpio/*.c
  ${APP_SRC_SUBSYS_DIR}/gpio/*.cpp
  ${APP_SRC_SUBSYS_DIR}/fs/fs_service.cpp
  ${APP_SRC_SUBSYS_DIR}/random/*.cpp
  ${APP_SRC_SUBSYS_DIR}/time/*.cpp
  ${APP_SRC_SUBSYS_DIR}/lua_script/*.cpp
  ${APP_SRC_SUBSYS_DIR}/math_parser/*.cpp
)

set(APP_SRC_SENSOR_DOMAIN_DIR ${APP_SRC_DIR}/domain/sensor_domain)
file(GLOB_RECURSE APP_SRC_SENSOR_DOMAIN_MODELS
  ${APP_SRC_SENSOR_DOMAIN_DIR}/configuration/*.c
  ${APP_SRC_SENSOR_DOMAIN_DIR}/configuration/*.cpp
  ${APP_SRC_SENSOR_DOMAIN_DIR}/models/*.c
  ${APP_SRC_SENSOR_DOMAIN_DIR}/models/*.cpp
)
file(GLOB_RECURSE APP_SRC_SENSOR_DOMAIN_UTILITIES
  ${APP_SRC_SENSOR_DOMAIN_DIR}/utilities/*.c
  ${APP_SRC_SENSOR_DOMAIN_DIR}/utilities/*.cpp
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
  ${APP_SRC_SUBSYS_DT}
  ${APP_SRC_SYSTEM_DOMAIN}
  ${APP_SRC_ADC_SUBSYS}
  ${APP_SRC_SUBSYS}
  ${APP_SRC_SENSOR_DOMAIN_MODELS}
  ${APP_SRC_SENSOR_DOMAIN_UTILITIES}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
  ${MUPARSER_SOURCES}
)
