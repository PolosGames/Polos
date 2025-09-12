cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

find_package(assimp CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

set(ThirdPartyLibs assimp::assimp ctre::ctre glfw Vulkan::Vulkan glm::glm imgui::imgui)


# Compile shaders to SPIR-V with GLSLC
find_program(GLSLC_EXECUTABLE glslc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found! Please ensure that the VULKAN_SDK environment variable is set correctly.")
endif()
message(STATUS "Found glslc: ${GLSLC_EXECUTABLE}")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Shaders)
set(SHADER_BINARY_DIR ${POLOS_INSTALL_DIR}/Shaders)
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
file(GLOB_RECURSE SHADER_SOURCE_FILES RELATIVE ${SHADER_SOURCE_DIR} ${SHADER_SOURCE_DIR}/*.vert ${SHADER_SOURCE_DIR}/*.frag)
set(SHADER_BINARY_FILES)
foreach(SHADER_SOURCE_FILE ${SHADER_SOURCE_FILES})
    set(SHADER_BINARY_FILE ${SHADER_BINARY_DIR}/${SHADER_SOURCE_FILE}.spv)
    add_custom_command(
        OUTPUT ${SHADER_BINARY_FILE}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE} -o ${SHADER_BINARY_FILE}
        DEPENDS ${SHADER_SOURCE_DIR}/${SHADER_SOURCE_FILE}
        COMMENT "Compiling shader: ${SHADER_SOURCE_FILE} to ${SHADER_BINARY_FILE}"
        VERBATIM
    )
    list(APPEND SHADER_BINARY_FILES ${SHADER_BINARY_FILE})
endforeach()
add_custom_target(Shaders ALL DEPENDS ${SHADER_BINARY_FILES})

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
