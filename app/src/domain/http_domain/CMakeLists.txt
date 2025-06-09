set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

foreach(web_resource
    index.html
    sensors_config_editor/index.html
    sensors_config_editor/style.css
    sensors_config_editor/editor.js
    sensors_config_editor/prism.css
    sensors_config_editor/prism.js
    )

    generate_inc_file_for_target(app static_resources/${web_resource} ${gen_dir}/${web_resource}.gz.inc --gzip)
endforeach()

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME http_resource_desc_http_service
                      KVMA RAM_REGION GROUP RODATA_REGION
                      SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})
