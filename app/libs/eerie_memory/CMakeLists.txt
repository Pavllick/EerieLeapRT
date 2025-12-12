set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(eerie_memory INTERFACE)

target_include_directories(eerie_memory
    INTERFACE
        ${INC_DIR}
)
