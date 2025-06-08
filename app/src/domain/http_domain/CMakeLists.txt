set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

set(source_file_index static_resources/index.html)
generate_inc_file_for_target(app ${source_file_index} ${gen_dir}/index.html.gz.inc --gzip)

set(source_file_sensors_config_editor static_resources/sensors_config_editor.html)
generate_inc_file_for_target(app ${source_file_sensors_config_editor} ${gen_dir}/sensors_config_editor.html.gz.inc --gzip)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME http_resource_desc_http_service
                      KVMA RAM_REGION GROUP RODATA_REGION
                      SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})
