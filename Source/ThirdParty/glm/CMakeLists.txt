cmake_minimum_required(VERSION 3.24)

add_library(glm_interface INTERFACE)
target_include_directories(glm_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(glm_static STATIC IMPORTED GLOBAL)
set_target_properties(
    glm_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/glm_static.lib"
            FOLDER            "ThirdParty"
)

add_library(glm::glm_static ALIAS glm_static)
add_library(glm::glm_interface ALIAS glm_interface)
