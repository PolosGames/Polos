vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO syoyo/tinygltf
    REF 97789b346ed792f413b9520c80bb479f3b63368f
    SHA512 3aac51f4c68e03ad11dec2ffc550c85535a0641a52815337c09d731c76800a89a4ee755308a4f550e478415db3d10b2159428912d1293d80c08da43f4625aa12
)

file(INSTALL
    "${SOURCE_PATH}/tiny_gltf_v3.h"
    "${SOURCE_PATH}/tiny_gltf_v3.c"
    "${SOURCE_PATH}/tinygltf_json_c.h"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
