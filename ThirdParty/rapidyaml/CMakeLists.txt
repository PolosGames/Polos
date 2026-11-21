# The upstream release archive includes the matching c4core and its submodules.
cpmaddpackage(
    NAME ryml
    VERSION 0.8.0
    URL https://github.com/biojppm/rapidyaml/releases/download/v0.8.0/rapidyaml-0.8.0-src.tgz
    URL_HASH SHA256=232476329f0937f16488fc0425070bf0176e686ed638f8b03fd96201ea08dac5
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "RYML_INSTALL OFF"
)

# Preserve the installed header paths already used by Polos.
set(ryml_include_dir "${CMAKE_CURRENT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${ryml_include_dir}/ryml")
foreach (header ryml.hpp ryml_std.hpp)
    configure_file("${ryml_SOURCE_DIR}/src/${header}" "${ryml_include_dir}/ryml/${header}" COPYONLY)
endforeach()
target_include_directories(
    ryml
        SYSTEM INTERFACE
            "${ryml_include_dir}"
)

install(DIRECTORY "${ryml_include_dir}/ryml" "${ryml_SOURCE_DIR}/src/c4"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY "${ryml_SOURCE_DIR}/ext/c4core/src/c4"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
    PATTERN "test*" EXCLUDE
    PATTERN "benchmarks" EXCLUDE
    PATTERN "fuzz" EXCLUDE
)

install(FILES "${ryml_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/rapidyaml"
    COMPONENT Runtime
)
install(FILES
    "${ryml_SOURCE_DIR}/ext/c4core/LICENSE.txt"
    "${ryml_SOURCE_DIR}/ext/c4core/LICENSE-BOOST.txt"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/c4core"
    COMPONENT Runtime
)
install(FILES "${ryml_SOURCE_DIR}/ext/c4core/src/c4/ext/debugbreak/COPYING"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/debugbreak"
    COMPONENT Runtime
)
install(FILES "${ryml_SOURCE_DIR}/ext/c4core/src/c4/ext/fast_float/LICENSE-MIT"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/fast_float"
    COMPONENT Runtime
)
