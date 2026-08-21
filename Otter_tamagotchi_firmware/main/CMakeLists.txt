file(GLOB_RECURSE FILES "*.c")
file(GLOB_RECURSE TSGL_FILES "../TSGL/TSGL/*.c")

idf_component_register(
    SRCS
        ${FILES}
        ${TSGL_FILES}

    INCLUDE_DIRS
        "."
        "../TSGL/TSGL"
)

fatfs_create_spiflash_image(storage ../storage FLASH_IN_PROJECT)