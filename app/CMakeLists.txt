cmake_minimum_required(VERSION 3.20)

# Bootstrap and build Boost
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/../modules/boost/b2")
    message(STATUS "Bootstrapping Boost...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/bootstrap.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE BOOTSTRAP_RESULT
    )
    if(NOT BOOTSTRAP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to bootstrap Boost")
    endif()

    message(STATUS "Building Boost headers and tools...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/b2 headers tools/bcp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE B2_HEADERS_RESULT
    )
    if(NOT B2_HEADERS_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to build Boost headers and tools")
    endif()

    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libs/boost)

    message(STATUS "Extracting Boost.Spirit.X3, Boost.Fusion and Boost.JSON headers...")
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/../modules/boost/dist/bin/bcp boost/spirit boost/fusion boost/json ${CMAKE_SOURCE_DIR}/libs/boost
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../modules/boost
        RESULT_VARIABLE BCP_RESULT
    )
    if(NOT BCP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to extract Boost.Spirit.X3, Boost.Fusion and Boost.JSON headers")
    endif()
endif()

# Apply patches before build
function(apply_patches patch_dir target_dir)
    file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "${patch_dir}/*.patch")
    foreach(file ${files})
        execute_process(
            COMMAND git apply --reverse --check ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
            WORKING_DIRECTORY ${target_dir}
            RESULT_VARIABLE patch_already_applied
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(patch_already_applied EQUAL 0)
            message("Patch already applied: ${file}")
        else()
            message("Applying patch: ${file}")
            execute_process(
                COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
                WORKING_DIRECTORY ${target_dir}
                RESULT_VARIABLE patch_apply_result
            )
            if(NOT patch_apply_result EQUAL 0)
                message(FATAL_ERROR "Failed to apply patch: ${file}")
            endif()
        endif()
    endforeach()
endfunction()

apply_patches("patches/zephyr" $ENV{ZEPHYR_BASE})

# ZCBOR code generation function
function(generate_zcbor_configuration cddl_path base_name)
    # Convert base_name to PascalCase for the type name
    string(REPLACE "_" ";" name_parts ${base_name})
    set(type_name "")
    foreach(part ${name_parts})
        string(SUBSTRING ${part} 0 1 first_char)
        string(TOUPPER ${first_char} first_char_upper)
        string(SUBSTRING ${part} 1 -1 rest_chars)
        string(APPEND type_name "${first_char_upper}${rest_chars}")
    endforeach()

    # Define output paths
    set(output_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/cbor/generated/${base_name}")
    set(output_cbor_h "${output_dir}/${base_name}_cbor.h")
    set(output_cbor_c "${output_dir}/${base_name}_cbor.c")
    set(output_cbor_decode_h "${output_dir}/${base_name}_cbor_decode.h")
    set(output_cbor_decode_c "${output_dir}/${base_name}_cbor_decode.c")
    set(output_cbor_encode_h "${output_dir}/${base_name}_cbor_encode.h")
    set(output_cbor_encode_c "${output_dir}/${base_name}_cbor_encode.c")
    set(output_types_h "${output_dir}/${base_name}.h")

    # Define the zcbor code generation command
    add_custom_command(
        OUTPUT
            ${output_cbor_decode_h}
            ${output_cbor_decode_c}
            ${output_cbor_encode_h}
            ${output_cbor_encode_c}
            ${output_types_h}
        COMMAND zcbor code -sed --default-max-qty 24
            -c ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/cbor/cbor_schemas/${cddl_path}
            -t ${type_name}
            --output-h ${output_cbor_h}
            --output-c ${output_cbor_c}
            --output-h-types ${output_types_h}
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/src/configuration/cbor/cbor_schemas/${cddl_path}
        COMMENT "Generating CBOR code from ${cddl_path} for ${base_name}"
        VERBATIM
    )

    # Create a custom target to trigger the code generation automatically
    add_custom_target(generate_${base_name}_cbor ALL
        DEPENDS
            ${output_cbor_decode_h}
            ${output_cbor_decode_c}
            ${output_cbor_encode_h}
            ${output_cbor_encode_c}
            ${output_types_h}
    )
endfunction()

# CBOR Serializer helpers
# generate_zcbor_configuration("adc_config.cddl" "cbor_adc_config")
# generate_zcbor_configuration("canbus_config.cddl" "cbor_canbus_config")
# generate_zcbor_configuration("logging_config.cddl" "cbor_logging_config")
# generate_zcbor_configuration("sensors_config.cddl" "cbor_sensors_config")
# generate_zcbor_configuration("system_config.cddl" "cbor_system_config")

# Add the Lua Zephyr module
set(EXTRA_ZEPHYR_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../modules/lua_zephyr")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(errie_leap)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++23 globally (Zephyr-compatible way)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Process nested HTTP domain CMake file
add_subdirectory(src/domain/http_domain)

add_definitions(-DBOOST_SYSTEM_DISABLE_THREADS)
add_definitions(-DBOOST_JSON_STACK_BUFFER_SIZE=1024)

# Collect all source files
file(GLOB_RECURSE APP_SOURCES
  src/*.cpp
  src/*.c
)

add_subdirectory(libs/dbcppp)
add_subdirectory(libs/muparser)
add_subdirectory(libs/nameof)
add_subdirectory(libs/eerie_memory)

target_link_libraries(app PRIVATE
    dbcppp
    muparser
    nameof
    eerie_memory
)

# Include headers
target_include_directories(app PRIVATE
  src
  libs/boost
)

# Add sources to the app target
target_sources(app PRIVATE ${APP_SOURCES})
