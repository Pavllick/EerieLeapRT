set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.c"
)

add_library(muparser ${SOURCES})

target_link_libraries(muparser PUBLIC zephyr_interface)

target_include_directories(muparser
    PUBLIC
        ${INC_DIR}
    PRIVATE
        ${SRC_DIR}
)

set_source_files_properties(${SRC} PROPERTIES LANGUAGE CXX)
