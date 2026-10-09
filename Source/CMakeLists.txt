cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# vcpkg also installs Vulkan headers as a transitive dependency of VMA/ImGui.
# Keep the headers paired with the SDK loader selected by FindVulkan.
if (UNIX AND NOT APPLE AND DEFINED ENV{VULKAN_SDK})
    include_directories(BEFORE "${Vulkan_INCLUDE_DIR}")
endif()

set(ThirdPartyLibs ctre::ctre glfw Vulkan::Vulkan glm::glm imgui::imgui GPUOpen::VulkanMemoryAllocator)

add_subdirectory(Resource)
add_subdirectory(Runtime)
add_subdirectory(ThirdParty)

# every module must be listed -- OBJECT libs don't propagate objects
# transitively, so depending on each other isn't enough to pull them in
add_library(polos SHARED)
target_link_libraries(
    polos
        PUBLIC
            polos::communication
            polos::core
            polos::filesystem
            polos::logging
            polos::platform
            polos::rendering
            polos::resource
            polos::utils
            ${ThirdPartyLibs}
)
set_target_properties(polos
    PROPERTIES
        CXX_STANDARD              ${POLOS_CXX_STANDARD}
        CXX_STANDARD_REQUIRED     True
        POSITION_INDEPENDENT_CODE True
        OUTPUT_NAME               "polos"
        CXX_VISIBILITY_PRESET     hidden
        VISIBILITY_INLINES_HIDDEN True
        LINKER_LANGUAGE           CXX
)
if (LINUX)
    get_filename_component(POLOS_VULKAN_LIBRARY_DIR "${Vulkan_LIBRARY}" DIRECTORY)
    set_target_properties(polos
        PROPERTIES
            INSTALL_RPATH "${POLOS_INSTALL_DIR};${POLOS_VULKAN_LIBRARY_DIR}"
            BUILD_WITH_INSTALL_RPATH 1
    )
endif()
add_library(polos::polos ALIAS polos)

install(
    TARGETS polos
    LIBRARY DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    ARCHIVE DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    RUNTIME DESTINATION "${POLOS_INSTALL_LIB_DIR}"
    PERMISSIONS OWNER_READ OWNER_WRITE
)

# linked here rather than in its own CMakeLists, where `polos` doesn't exist yet
if (HOT_RELOAD)
    target_link_libraries(rendering_impl PRIVATE polos)
    if (LINUX)
        set_target_properties(rendering_impl PROPERTIES
            INSTALL_RPATH "${POLOS_INSTALL_DIR};${POLOS_VULKAN_LIBRARY_DIR}"
        )
    endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    CheckConfigurationExist(Release DOES_EXIST)
    if (NOT ${DOES_EXIST})
        message(FATAL_ERROR "Release preset needs to be configured in order to build with RelWithDebInfo")
    endif()
endif()

# IMPORTED_RUNTIME_ARTIFACTS picks the right config itself and installs the whole
# SONAME symlink chain. Copying IMPORTED_LOCATION as a plain file dropped
# libglfw.so.3.4 without libglfw.so.3, and the loader then fell through to
# whatever glfw the system had.
foreach(LIB ${ThirdPartyLibs})
    get_target_property(LIB_TYPE ${LIB} TYPE)
    if (LIB_TYPE STREQUAL "SHARED_LIBRARY")
        install(
            IMPORTED_RUNTIME_ARTIFACTS ${LIB}
            LIBRARY DESTINATION ${POLOS_INSTALL_DIR}
            RUNTIME DESTINATION ${POLOS_INSTALL_DIR}
        )
    endif()
endforeach()
