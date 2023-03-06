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
            PolosThirdParty
            Runtime
            fmod::fmod_interface
            glad::glad_interface
            glfw::glfw_interface
            glm::glm_interface
            ImGui::ImGui_interface
            spdlog::spdlog_interface
            stb_image::stb_image_interface
            stb_textedit::stb_textedit_interface
)

set_target_properties(
    Polos
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
            PRECOMPILE_HEADERS_REUSE_FROM ${PCH_LIBRARY_NAME}
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Polos)
endif()

build_options(Polos true)

install(
    TARGETS Polos
    DESTINATION ${INSTALL_DIR}
)