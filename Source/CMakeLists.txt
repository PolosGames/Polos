cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

find_package(assimp CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

set(ThirdPartyLibs assimp::assimp ctre::ctre glfw Vulkan::Vulkan glm::glm imgui::imgui)

add_subdirectory(Resource)
add_subdirectory(Runtime)
add_subdirectory(ThirdParty)

add_library(polos INTERFACE)
target_link_libraries(
    polos
        INTERFACE
            polos::communication
            polos::core
            polos::datatypes
            polos::logging
            polos::platform
            polos::rendering
            polos::utils
            ${ThirdPartyLibs}
)
add_library(polos::polos ALIAS polos)

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
