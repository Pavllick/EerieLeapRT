set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(leap_memory INTERFACE)

target_include_directories(leap_memory
    INTERFACE
        ${INC_DIR}
)
