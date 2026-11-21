cpmaddpackage(
    NAME simdjson
    VERSION 5.1.0
    URL https://codeload.github.com/simdjson/simdjson/tar.gz/refs/tags/v5.1.0
    URL_HASH SHA256=f413941e66142ade07adfca7c277f092f020560ea971db33414d95170afe93c4
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "SIMDJSON_INSTALL OFF" "SIMDJSON_DEVELOPMENT_CHECKS ON"
)

cpmaddpackage(
    NAME simdutf
    VERSION 9.3.3
    URL https://codeload.github.com/simdutf/simdutf/tar.gz/refs/tags/v9.3.3
    URL_HASH SHA256=441e374e8629bd8469f6d5cde9f0fa17ef3ad158aeb077791606d8aa73b565ef
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "SIMDUTF_TESTS OFF" "SIMDUTF_TOOLS OFF"
)

cpmaddpackage(
    NAME fastgltf
    VERSION 1.0.1
    URL https://codeload.github.com/spnda/fastgltf/tar.gz/refs/tags/v1.0.1
    URL_HASH SHA256=8399c5ed2cd90d0aef0704e65c02be59f866a9baf56bdc310cb647a6744ead94
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "FASTGLTF_ENABLE_INSTALL OFF"
)

install(FILES "${fastgltf_SOURCE_DIR}/LICENSE.md"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/fastgltf"
    COMPONENT Runtime
)
install(FILES "${simdjson_SOURCE_DIR}/LICENSE" "${simdjson_SOURCE_DIR}/LICENSE-MIT"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/simdjson"
    COMPONENT Runtime
)
install(FILES "${simdutf_SOURCE_DIR}/LICENSE-APACHE" "${simdutf_SOURCE_DIR}/LICENSE-MIT"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/simdutf"
    COMPONENT Runtime
)
