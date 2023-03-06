cmake_minimum_required(VERSION 3.24)

add_library(glfw_static STATIC IMPORTED)
set_target_properties(
    glfw_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/glfw.lib"
            FOLDER            "ThirdParty"
)

add_library(glfw_interface INTERFACE)
target_include_directories(glfw_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(glfw::glfw_static ALIAS glfw_static)
add_library(glfw::glfw_interface ALIAS glfw_interface)