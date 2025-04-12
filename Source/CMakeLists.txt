cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_subdirectory(ThirdParty)
add_subdirectory(Runtime)

add_library(polos_interface INTERFACE)
target_include_directories(polos_interface INTERFACE ${POLOS_INC_DIR})
add_library(polos::polos_interface ALIAS polos_interface)

add_library(polos INTERFACE)
target_link_libraries(
    polos
        INTERFACE
            polos::polos_interface
            polos::polos_core
            polos::polos_datatypes
            polos::polos_logging
            #polos::polos_platform
            #polos::polos_backend
)
add_library(polos::polos ALIAS polos)

if(BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

