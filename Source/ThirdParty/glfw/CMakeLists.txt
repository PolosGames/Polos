cmake_minimum_required(VERSION 3.24)

add_library(glfw_interface INTERFACE)
target_include_directories(glfw_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    set(LIB_NAME "${CMAKE_BUILD_TYPE}/glfw.lib")
elseif(LINUX)
    set(LIB_NAME "libglfw.so")
endif()

set(LIB_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${LIB_NAME}")

add_library(glfw_static STATIC IMPORTED GLOBAL)
set_target_properties(
    glfw_static
        PROPERTIES
            IMPORTED_LOCATION "${LIB_LOCATION}"
            FOLDER            "ThirdParty"
)

add_library(glfw::glfw_static ALIAS glfw_static)
add_library(glfw::glfw_interface ALIAS glfw_interface)
