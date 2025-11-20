if(CONFIG_LUA)
    set(LUA_DIR "${ZEPHYR_CURRENT_MODULE_DIR}/")
    set(INC_DIR "${LUA_DIR}/include")
    set(SRC_DIR "${LUA_DIR}/src")

    zephyr_include_directories("${INC_DIR}")

    zephyr_library()

    file(GLOB_RECURSE SRC "${SRC_DIR}/*.c")

    zephyr_library_sources(${SRC})

    # Compile Lua sources as C++
    set_source_files_properties(
        ${SRC}
        PROPERTIES LANGUAGE CXX
    )
endif()
