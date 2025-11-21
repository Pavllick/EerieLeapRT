set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(nameof INTERFACE)

target_include_directories(nameof
    INTERFACE
        ${INC_DIR}
)
