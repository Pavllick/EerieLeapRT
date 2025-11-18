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
set(BOOST_INCLUDE_DIR ${APP_SRC_DIR}/../include/boost)
include_directories(${BOOST_INCLUDE_DIR})

# Path to boost include directory
set(BOOST_INCLUDE_DIR ${APP_SRC_DIR}/../include/boost)
include_directories(${BOOST_INCLUDE_DIR})

# Create Boost.JSON library
add_library(boost_json STATIC ${APP_SRC_DIR}/../../modules/boost/libs/json/src/src.cpp)
target_include_directories(boost_json PUBLIC include/boost)
# Copy compiler properties from app target to ensure same architecture
target_compile_options(boost_json PRIVATE $<TARGET_PROPERTY:app,COMPILE_OPTIONS>)
target_compile_definitions(boost_json PRIVATE $<TARGET_PROPERTY:app,COMPILE_DEFINITIONS>)

# Link Boost.JSON library
target_link_libraries(app PRIVATE boost_json)
add_definitions(-DBOOST_SYSTEM_DISABLE_THREADS)

# Path to muparser source and header
set(MUPARSER_SRC_DIR ${APP_SRC_DIR}/libs/muparser/src)
set(MUPARSER_INCLUDE_DIR ${APP_SRC_DIR}/libs/muparser/include)
file(GLOB_RECURSE MUPARSER_SOURCES ${MUPARSER_SRC_DIR}/*.cpp)
include_directories(${MUPARSER_INCLUDE_DIR})

# Path to nameof source and header
set(NAMEOF_INCLUDE_DIR ${APP_SRC_DIR}/libs/nameof/include)
include_directories(${NAMEOF_INCLUDE_DIR})

# Path to dbcppp source and header
set(DBCPPP_SRC_DIR ${APP_SRC_DIR}/libs/dbcppp/src)
set(DBCPPP_INCLUDE_DIR ${APP_SRC_DIR}/libs/dbcppp/include)
file(GLOB_RECURSE DBCPPP_SOURCES ${DBCPPP_SRC_DIR}/*.cpp)
include_directories(${DBCPPP_INCLUDE_DIR})

set(APP_SRC_CONFIGURATION_DIR ${APP_SRC_DIR}/configuration)
file(GLOB_RECURSE APP_SRC_CONFIGURATION
  ${APP_SRC_CONFIGURATION_DIR}/*.c
  ${APP_SRC_CONFIGURATION_DIR}/*.cpp
)

set(APP_SRC_SUBSYS_DIR ${APP_SRC_DIR}/subsys)
file(GLOB_RECURSE APP_SRC_SUBSYS
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_fs.cpp
  ${APP_SRC_SUBSYS_DIR}/device_tree/dt_logger.cpp
  ${APP_SRC_SUBSYS_DIR}/adc/*.c
  ${APP_SRC_SUBSYS_DIR}/adc/*.cpp
  ${APP_SRC_SUBSYS_DIR}/gpio/*.c
  ${APP_SRC_SUBSYS_DIR}/gpio/*.cpp
  ${APP_SRC_SUBSYS_DIR}/fs/fs_service.cpp
  ${APP_SRC_SUBSYS_DIR}/random/*.cpp
  ${APP_SRC_SUBSYS_DIR}/time/*.cpp
  ${APP_SRC_SUBSYS_DIR}/dbc/*.cpp
)

set(APP_SRC_DOMAIN_DIR ${APP_SRC_DIR}/domain)
file(GLOB_RECURSE APP_SRC_DOMAIN
  ${APP_SRC_DOMAIN_DIR}/sensor_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/sensor_domain/*.cpp
  ${APP_SRC_DOMAIN_DIR}/system_domain/*.c
  ${APP_SRC_DOMAIN_DIR}/system_domain/*.cpp
)

set(APP_SRC_UTILITIES_DIR ${APP_SRC_DIR}/utilities)
file(GLOB_RECURSE APP_SRC_UTILITIES
  ${APP_SRC_UTILITIES_DIR}/*.c
  ${APP_SRC_UTILITIES_DIR}/*.cpp
)

file(GLOB_RECURSE TEST_SOURCES src/*.c src/*.cpp)

# Add all source files to the app target
target_sources(app PRIVATE
  ${APP_SRC_CONFIGURATION}
  ${APP_SRC_SUBSYS}
  ${APP_SRC_DOMAIN}
  ${APP_SRC_UTILITIES}
  ${TEST_SOURCES}
  ${MUPARSER_SOURCES}
  ${DBCPPP_SOURCES}
)
