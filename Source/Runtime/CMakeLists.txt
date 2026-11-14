cmake_minimum_required(VERSION 3.25)

find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan 1.3 REQUIRED)
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

if (DEFINED ENV{VULKAN_SDK})
    target_include_directories(polos BEFORE PUBLIC "${Vulkan_INCLUDE_DIR}")
endif()

target_link_libraries(polos PUBLIC
    ctre::ctre
    glfw
    glm::glm
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    quill::quill_shared
    stb_image::stb_image
    stb_textedit::stb_textedit
    fastgltf::fastgltf
)
if (ENABLE_PROFILING)
    target_link_libraries(polos PUBLIC Tracy::TracyClient)
endif()

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
if (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # libstdc++ keeps the Windows terminal helpers used by std::println here.
    target_link_libraries(polos PRIVATE stdc++exp)
endif()

add_subdirectory(polos/communication)
add_subdirectory(polos/core)
add_subdirectory(polos/ecs)
add_subdirectory(polos/filesystem)
add_subdirectory(polos/gui)
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
    LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime NAMELINK_COMPONENT SDK
    ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT SDK
    RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime
    FILE_SET runtime_headers DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK
    FILE_SET generated_headers DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK
)
if (MSVC)
    install(FILES "$<TARGET_PDB_FILE:polos>" DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime OPTIONAL)
endif()
install(FILES "${CMAKE_SOURCE_DIR}/VERSION" "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION "${POLOS_INSTALL_DIR}" COMPONENT Runtime)

set(POLOS_VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
install(DIRECTORY
    "${POLOS_VCPKG_INCLUDE_DIR}/glm"
    "${POLOS_VCPKG_INCLUDE_DIR}/quill"
    "${POLOS_VCPKG_INCLUDE_DIR}/GLFW"
    "${Vulkan_INCLUDE_DIR}/vulkan"
    "${Vulkan_INCLUDE_DIR}/vk_video"
    DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK)
get_target_property(POLOS_VMA_INCLUDE_HINTS GPUOpen::VulkanMemoryAllocator INTERFACE_INCLUDE_DIRECTORIES)
find_path(POLOS_VMA_INCLUDE_DIR vk_mem_alloc.h
    HINTS ${POLOS_VMA_INCLUDE_HINTS} "${POLOS_VCPKG_INCLUDE_DIR}" "${Vulkan_INCLUDE_DIR}"
    PATH_SUFFIXES vma REQUIRED)
install(FILES "${POLOS_VMA_INCLUDE_DIR}/vk_mem_alloc.h"
    DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK)

foreach(DEPENDENCY glfw glm::glm imgui::imgui)
    get_target_property(DEPENDENCY_TYPE ${DEPENDENCY} TYPE)
    if (DEPENDENCY_TYPE STREQUAL "SHARED_LIBRARY")
        install(IMPORTED_RUNTIME_ARTIFACTS ${DEPENDENCY}
            LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime
            RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}" COMPONENT Runtime
        )
    endif()
endforeach()
