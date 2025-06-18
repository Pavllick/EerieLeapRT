cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_logger_test)

target_include_directories(app PRIVATE ../../../../app/src)

set(APP_SOURCES
    ../../../../app/src/utilities/logging/logging_core.cpp # Updated path
)

target_sources(app PRIVATE
    src/app_logger_test.cpp
    ${APP_SOURCES}
)
