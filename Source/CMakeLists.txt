cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(QUILL_INCLUDE_DIRS "quill/Backend.h")

add_subdirectory(ThirdParty)

set(POLOS_PCH_LIB PolosPCH)

file(WRITE polos.cpp "")
add_library(${POLOS_PCH_LIB} STATIC "polos.cpp")

target_precompile_headers(${POLOS_PCH_LIB} PUBLIC polos_pch.h)
target_link_libraries(${POLOS_PCH_LIB} PRIVATE glm::glm-header-only)

add_subdirectory(Runtime)
add_subdirectory(Editor)
