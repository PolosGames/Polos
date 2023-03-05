cmake_minimum_required(VERSION 3.24)

add_executable(Polos)

target_sources(
    Polos
        PRIVATE
            editor.cpp
)

target_include_directories(
    Polos
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    Polos
        PRIVATE
            glm_static
            glfw
            ImGui
            glad
            Runtime
)

set_target_properties(
    Polos
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Polos)
endif()

build_options(Polos true)

install(
    TARGETS Polos
    DESTINATION ${INSTALL_DIR}
)