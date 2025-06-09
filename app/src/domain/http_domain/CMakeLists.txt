set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

set(editor_dir ${CMAKE_CURRENT_SOURCE_DIR}/static_resources/sensors_config_editor)
set(bundled_file ${ZEPHYR_BINARY_DIR}/include/generated/static_resources/sensors_config_editor/bundle.html)

add_custom_command(
    OUTPUT ${bundled_file}
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle_html.py
        --html ${editor_dir}/index.html
        --output ${bundled_file}
        --base ${editor_dir}
    DEPENDS
        ${editor_dir}/index.html
        ${editor_dir}/style.css
        ${editor_dir}/editor.js
        ${editor_dir}/prism.css
        ${editor_dir}/prism.js
    COMMENT "Bundling sensors_config_editor resources into single HTML"
)

generate_inc_file_for_target(app
    ${bundled_file}
    ${gen_dir}/sensors_config_editor/bundle.html.gz.inc
    --gzip
)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME http_resource_desc_http_service
                      KVMA RAM_REGION GROUP RODATA_REGION
                      SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})
