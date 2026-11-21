if (CMAKE_CROSSCOMPILING AND MINGW)
    set(vulkan_sdk_tag vulkan-sdk-1.4.304.1)
    cpmaddpackage(
        NAME vulkan_headers
        VERSION 1.4.304.1
        URL https://codeload.github.com/KhronosGroup/Vulkan-Headers/tar.gz/refs/tags/${vulkan_sdk_tag}
        URL_HASH SHA256=7db4ea1c9c0860a1c1c9ccf34477f496d2929f8e705423f951131921bedfb51c
        DOWNLOAD_ONLY YES
    )
    cpmaddpackage(
        NAME vulkan_loader
        VERSION 1.4.304.1
        URL https://codeload.github.com/KhronosGroup/Vulkan-Loader/tar.gz/refs/tags/${vulkan_sdk_tag}
        URL_HASH SHA256=efb2c84da071765a8b0b15e9c60801d4611ef7501aa8b3d1268c3b779bfcfaf6
        DOWNLOAD_ONLY YES
    )

    # The target uses the Vulkan loader installed on Windows. Generate its MinGW import library.
    set(vulkan_include_dir "${vulkan_headers_SOURCE_DIR}/include")
    set(vulkan_exports "${vulkan_loader_SOURCE_DIR}/loader/vulkan-1.def")
    set(vulkan_import_library "${CMAKE_CURRENT_BINARY_DIR}/libvulkan-1.dll.a")
    add_custom_command(
        OUTPUT "${vulkan_import_library}"
        COMMAND "${CMAKE_DLLTOOL}" --input-def "${vulkan_exports}" --output-lib "${vulkan_import_library}"
        DEPENDS "${vulkan_exports}"
        VERBATIM
    )
    add_custom_target(
        polos_vulkan_import
            DEPENDS "${vulkan_import_library}"
    )

    add_library(
        Vulkan::Vulkan
            SHARED IMPORTED GLOBAL
    )
    set_target_properties(
        Vulkan::Vulkan
            PROPERTIES
                IMPORTED_IMPLIB "${vulkan_import_library}"
                INTERFACE_INCLUDE_DIRECTORIES "${vulkan_include_dir}"
    )
    add_dependencies(
        Vulkan::Vulkan
            polos_vulkan_import
    )
else()
    find_package(Vulkan 1.3 REQUIRED GLOBAL)
    set(vulkan_include_dir "${Vulkan_INCLUDE_DIR}")
endif()

find_program(POLOS_GLSLANG_EXECUTABLE
    NAMES glslang glslangValidator
    HINTS "$ENV{VULKAN_SDK}/bin"
    NO_CMAKE_FIND_ROOT_PATH
    REQUIRED
)

install(DIRECTORY
    "${vulkan_include_dir}/vulkan"
    "${vulkan_include_dir}/vk_video"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
)

# System Vulkan installations may omit license files next to the SDK headers.
install(FILES LICENSE.txt
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/vulkan"
    COMPONENT SDK
)
