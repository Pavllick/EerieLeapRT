set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/static_resources/)

set(http_configuration_dir ${CMAKE_CURRENT_SOURCE_DIR}/static_resources/configuration)

add_custom_command(
    OUTPUT ${gen_dir}/configuration/editor_bundle.html
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle_html.py
        --html ${http_configuration_dir}/editor.html
        --output ${gen_dir}/configuration/editor_bundle.html
        --base ${http_configuration_dir}
    DEPENDS
        ${http_configuration_dir}/editor.html
        ${http_configuration_dir}/styles/style.css
        ${http_configuration_dir}/scripts/editor.js
        ${http_configuration_dir}/styles/prism.css
        ${http_configuration_dir}/scripts/prism.js
    COMMENT "Bundling sensors_config_editor resources into single HTML"
)

add_custom_command(
    OUTPUT ${gen_dir}/configuration/editor_bundle.html.min
    COMMAND html-minifier-terser
        --collapse-whitespace
        --remove-comments
        --remove-attribute-quotes
        --remove-redundant-attributes
        --use-short-doctype
        --sort-attributes
        --sort-class-name
        --minify-js
        --minify-css
        --ignore-custom-fragments '<code class="language-json">[\\s\\S]*?<\\/code>'
        -o ${gen_dir}/configuration/editor_bundle.html.min ${gen_dir}/configuration/editor_bundle.html
    DEPENDS ${gen_dir}/configuration/editor_bundle.html
    COMMENT "Minifying sensors_config_editor HTML"
)

generate_inc_file_for_target(app
    ${gen_dir}/configuration/editor_bundle.html.min
    ${gen_dir}/configuration/editor_bundle.html.gz.inc
    --gzip
)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section(NAME http_resource_desc_http_service
                      KVMA RAM_REGION GROUP RODATA_REGION
                      SUBALIGN ${CONFIG_LINKER_ITERABLE_SUBALIGN})
