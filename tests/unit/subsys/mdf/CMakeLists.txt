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
  ${APP_SRC_DIR}/../libs/dbcppp
  ${CMAKE_CURRENT_BINARY_DIR}/dbcppp)

target_link_libraries(app PRIVATE
    dbcppp
)

set(APP_SRC_SUBSYS_DIR ${APP_SRC_DIR}/subsys)
file(GLOB_RECURSE APP_SRC_SUBSYS
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_logger.cpp
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_fs.cpp
  ${APP_SRC_SUBSYS_DIR}/random/*.c
  ${APP_SRC_SUBSYS_DIR}/random/*.cpp
  ${APP_SRC_SUBSYS_DIR}/time/*.cpp
  ${APP_SRC_SUBSYS_DIR}/fs/*.c
  ${APP_SRC_SUBSYS_DIR}/fs/*.cpp
  ${APP_SRC_SUBSYS_DIR}/mdf/*.c
  ${APP_SRC_SUBSYS_DIR}/mdf/*.cpp
)

set(APP_SRC_UTILITIES_DIR ${APP_SRC_DIR}/utilities)
file(GLOB_RECURSE APP_SRC_UTILITIES
  ${APP_SRC_UTILITIES_DIR}/memory/*.c
  ${APP_SRC_UTILITIES_DIR}/memory/*.cpp
  ${APP_SRC_UTILITIES_DIR}/string/*.c
  ${APP_SRC_UTILITIES_DIR}/string/*.cpp
  ${APP_SRC_UTILITIES_DIR}/time/*.c
  ${APP_SRC_UTILITIES_DIR}/time/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_SUBSYS}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
  ${DBCPPP_SOURCES}
)
