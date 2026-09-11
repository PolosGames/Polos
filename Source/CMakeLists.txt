cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

#find_package(assimp CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

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
    set_target_properties(polos
        PROPERTIES
            INSTALL_RPATH ${POLOS_INSTALL_DIR}
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
endif()

foreach(LIB ${ThirdPartyLibs})
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        get_target_property(LIB_SHARED_LIBRARIES ${LIB} IMPORTED_LOCATION_RELEASE)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        get_target_property(LIB_SHARED_LIBRARIES ${LIB} IMPORTED_LOCATION_DEBUG)
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        CheckConfigurationExist(Release DOES_EXIST)
        if (NOT ${DOES_EXIST})
            message(FATAL_ERROR "Release preset needs to be configured in order to build with RelWithDebInfo")
        endif()
        get_target_property(LIB_SHARED_LIBRARIES ${LIB} IMPORTED_LOCATION_RELEASE)
    endif()
    if(LIB_SHARED_LIBRARIES)
        install(FILES ${LIB_SHARED_LIBRARIES} DESTINATION ${POLOS_INSTALL_DIR})
    endif()
endforeach()
