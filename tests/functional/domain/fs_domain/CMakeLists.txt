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

set(APP_SRC_SUBSYS_DT_DIR ${APP_SRC_DIR}/subsys/device_tree)
file(GLOB_RECURSE APP_SRC_SUBSYS_DT
  ${APP_SRC_SUBSYS_DT_DIR}/dt_fs.cpp
)

set(APP_SRC_SUBSYS_FS_DIR ${APP_SRC_DIR}/subsys/fs)
file(GLOB_RECURSE APP_SRC_SUBSYS_FS
  ${APP_SRC_SUBSYS_FS_DIR}/*.c
  ${APP_SRC_SUBSYS_FS_DIR}/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_SUBSYS_DT}
  ${APP_SRC_SUBSYS_FS}
  ${TEST_SOURCES}
)
