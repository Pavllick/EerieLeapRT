set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BOOST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../boost")

file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.c"
)

add_library(dbcppp ${SOURCES})

target_link_libraries(dbcppp PUBLIC
    zephyr_interface
)

target_include_directories(dbcppp
    PUBLIC
        ${INC_DIR}
    PRIVATE
        ${SRC_DIR}
        ${BOOST_DIR}
        ${EERIE_MEMORY_DIR}
)

set_source_files_properties(${SRC} PROPERTIES LANGUAGE CXX)
