include(cpm/Bootstrap.cmake)

# Dependencies linked into the runtime are built with the same toolchain and PIC.
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(vulkan)

if (LINUX)
    set(glfw_version 3.5.1)
    set(glfw_sha256 5234f4f29473e9a06bc7847d8371858dd135d38466eeeaa652fdc9f8f9ff0c20)
    set(glfw_options "GLFW_BUILD_WAYLAND ON" "GLFW_BUILD_X11 ON")
else()
    set(glfw_version 3.4)
    set(glfw_sha256 c038d34200234d071fae9345bc455e4a8f2f544ab60150765d7704e08f3dac01)
    if (WIN32 AND NOT MINGW)
        set(glfw_options "BUILD_SHARED_LIBS ON")
    endif()
endif()

cpmaddpackage(
    NAME glfw
    VERSION ${glfw_version}
    URL https://codeload.github.com/glfw/glfw/tar.gz/refs/tags/${glfw_version}
    URL_HASH SHA256=${glfw_sha256}
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS
        "GLFW_BUILD_DOCS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_INSTALL OFF"
        ${glfw_options}
)

add_library(
    glfw::glfw
        ALIAS glfw
)

cpmaddpackage(
    NAME glm
    VERSION 1.0.1
    URL https://codeload.github.com/g-truc/glm/tar.gz/refs/tags/1.0.1
    URL_HASH SHA256=9f3174561fd26904b23f0db5e560971cbf9b3cbda0b280f04d5c379d03bf234c
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
    OPTIONS "GLM_BUILD_INSTALL OFF"
)

cpmaddpackage(
    NAME vma
    VERSION 3.1.0
    URL https://codeload.github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/tar.gz/refs/tags/v3.1.0
    URL_HASH SHA256=ae134ecc37c55634f108e926f85d5d887b670360e77cd107affaf3a9539595f2
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
)

add_subdirectory(quill)
add_subdirectory(rapidyaml)
add_subdirectory(stb_image)
add_subdirectory(fastgltf)
add_subdirectory(imgui)
set(POLOS_IMGUI_BACKEND_DIR "${POLOS_IMGUI_BACKEND_DIR}" PARENT_SCOPE)

if (BUILD_TESTS)
    cpmaddpackage(
        NAME googletest
        VERSION 1.16.0
        URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.16.0
        URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
        SYSTEM YES
        EXCLUDE_FROM_ALL YES
        OPTIONS "INSTALL_GTEST OFF" "BUILD_GMOCK OFF" "gtest_force_shared_crt ON"
    )
    cpmaddpackage(
        NAME nlohmann_json
        VERSION 3.11.3
        URL https://codeload.github.com/nlohmann/json/tar.gz/refs/tags/v3.11.3
        URL_HASH SHA256=0d8ef5af7f9794e3263480193c491549b2ba6cc74bb018906202ada498a79406
        SYSTEM YES
        EXCLUDE_FROM_ALL YES
        OPTIONS "JSON_BuildTests OFF" "JSON_Install OFF"
    )
endif()

if (ENABLE_PROFILING)
    add_subdirectory(tracy)
endif()

install(DIRECTORY
    "${glfw_SOURCE_DIR}/include/GLFW"
    "${glm_SOURCE_DIR}/glm"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
)
install(FILES "${vma_SOURCE_DIR}/include/vk_mem_alloc.h"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
)

# Package notices for dependencies shipped with Polos. CPM supplies upstream copies.
install(FILES "${glfw_SOURCE_DIR}/LICENSE.md"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/glfw"
    COMPONENT Runtime
)
install(FILES "${glm_SOURCE_DIR}/copying.txt"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/glm"
    COMPONENT Runtime
)
install(FILES "${vma_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/vma"
    COMPONENT Runtime
)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/stb_image/license.txt"
    DESTINATION "${POLOS_INSTALL_DIR}/licenses/stb_image"
    COMPONENT Runtime
)

get_target_property(glfw_type glfw TYPE)
if (glfw_type STREQUAL "SHARED_LIBRARY")
    install(TARGETS glfw
        EXPORT PolosTargets
        RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime
        LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime NAMELINK_COMPONENT SDK
        ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT SDK
    )
endif()

# Append suppression after dependencies have set their own warning flags.
function(polos_suppress_dependency_warnings directory)
    if (MSVC)
        set(suppress_flag /w)
    else()
        set(suppress_flag -w)
    endif()

    get_property(targets DIRECTORY "${directory}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach (target IN LISTS targets)
        get_target_property(type ${target} TYPE)
        if (type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$")
            target_compile_options(
                ${target}
                    PRIVATE ${suppress_flag}
            )
        endif()
    endforeach()

    get_property(subdirectories DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
    foreach (subdirectory IN LISTS subdirectories)
        polos_suppress_dependency_warnings("${subdirectory}")
    endforeach()
endfunction()

polos_suppress_dependency_warnings("${CMAKE_CURRENT_SOURCE_DIR}")
