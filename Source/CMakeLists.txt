cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

find_package(assimp CONFIG REQUIRED)
find_package(ctre CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

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
            polos::utils
)
add_library(polos::polos ALIAS polos)

if(${BUILD_EDITOR})
    add_subdirectory(Editor)
endif()
