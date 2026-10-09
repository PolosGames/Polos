cmake_minimum_required(VERSION 3.25)

find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

add_library(polos SHARED)
add_library(polos::polos ALIAS polos)
target_compile_features(polos PUBLIC cxx_std_23)
target_compile_definitions(polos PRIVATE POLOS_ENGINE_EXPORTS PL_LOGGER_TYPE=Polos QUILL_DLL_IMPORT)
target_sources(polos
    PUBLIC
        FILE_SET runtime_headers TYPE HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILE_SET generated_headers TYPE HEADERS
            BASE_DIRS "${POLOS_CONFIG_INCLUDE_DIR}"
            FILES
                "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_api.hpp"
                "${POLOS_CONFIG_INCLUDE_DIR}/polos/polos_config.hpp"
)
# Pair headers with the SDK loader instead of vcpkg's transitive Vulkan headers.
if (UNIX AND NOT APPLE AND DEFINED ENV{VULKAN_SDK})
    target_include_directories(polos BEFORE PUBLIC "${Vulkan_INCLUDE_DIR}")
endif()

target_link_libraries(polos PUBLIC
    ctre::ctre
    glfw
    glm::glm
    imgui::imgui
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    quill::quill_shared
    stb_image::stb_image
    stb_textedit::stb_textedit
    tinygltf::tinygltf
)
set_target_properties(polos
    PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     ON
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN ON
        VERSION                   ${PROJECT_VERSION}
        SOVERSION                 ${PROJECT_VERSION_MAJOR}
        INSTALL_RPATH             "$ORIGIN"
)
build_options(polos true)

add_subdirectory(polos/communication)
add_subdirectory(polos/core)
add_subdirectory(polos/ecs)
add_subdirectory(polos/filesystem)
add_subdirectory(polos/logging)
add_subdirectory(polos/platform)
add_subdirectory(polos/rendering)
add_subdirectory(polos/resource)
add_subdirectory(polos/utils)

get_target_property(POLOS_SOURCES polos SOURCES)
get_target_property(POLOS_RUNTIME_HEADERS polos HEADER_SET_runtime_headers)
get_target_property(POLOS_GENERATED_HEADERS polos HEADER_SET_generated_headers)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Runtime
    FILES ${POLOS_SOURCES} ${POLOS_RUNTIME_HEADERS})
source_group(Generated FILES ${POLOS_GENERATED_HEADERS})
polos_attach_build_info(polos "${POLOS_SOURCES};${POLOS_RUNTIME_HEADERS};${POLOS_GENERATED_HEADERS}")

add_dependencies(polos compile_shaders)
install(TARGETS polos
    LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    FILE_SET runtime_headers DESTINATION "${POLOS_INSTALL_INC_DIR}"
    FILE_SET generated_headers DESTINATION "${POLOS_INSTALL_INC_DIR}"
)
install(FILES "${CMAKE_SOURCE_DIR}/VERSION" DESTINATION "${POLOS_INSTALL_DIR}")

# Install shared third-party runtime artifacts with their SONAME symlink chains.
foreach(DEPENDENCY ctre::ctre glfw glm::glm imgui::imgui GPUOpen::VulkanMemoryAllocator)
    get_target_property(DEPENDENCY_TYPE ${DEPENDENCY} TYPE)
    if (DEPENDENCY_TYPE STREQUAL "SHARED_LIBRARY")
        install(IMPORTED_RUNTIME_ARTIFACTS ${DEPENDENCY}
            LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}"
            RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}"
        )
    endif()
endforeach()
