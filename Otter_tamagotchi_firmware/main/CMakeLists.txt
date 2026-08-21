file(GLOB_RECURSE FILES "*.c")
file(GLOB_RECURSE TSGL_FILES "../TSGL-0.3.3/TSGL/*.c")

idf_component_register(
    SRCS
        ${FILES}
        ${TSGL_FILES}

    INCLUDE_DIRS
        "."
        "../TSGL-0.3.3/TSGL"
)

fatfs_create_spiflash_image(storage ../storage FLASH_IN_PROJECT)