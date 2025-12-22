cmake_minimum_required(VERSION 3.20.0)

set(EXTRA_ZEPHYR_MODULES
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/lua_zephyr"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/eerie_leap_common")

# Use Zephyr's build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_guid_generator)

# Enable C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src)
include_directories(${APP_SRC_DIR})

set(APP_SRC_DOMAIN_DIR ${APP_SRC_DIR}/domain)
file(GLOB_RECURSE APP_SRC_DOMAIN
  ${APP_SRC_DOMAIN_DIR}/sensor_domain/utilities/sensors_order_resolver.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_DOMAIN}
  ${TEST_SOURCES}
)
