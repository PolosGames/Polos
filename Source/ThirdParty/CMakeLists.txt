cmake_minimum_required(VERSION 3.24)

add_subdirectory(fmod)
if(USE_OPENGL)
    add_subdirectory(glad)
endif()
if(USE_OPENGL OR USE_VULKAN)
    add_subdirectory(glfw)
endif()
add_subdirectory(glm)
if(GUI_USE_IMGUI)
    add_subdirectory(imgui)
endif()
add_subdirectory(spdlog)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)

add_library(PolosThirdParty INTERFACE)

target_link_libraries(
    PolosThirdParty
        INTERFACE
            fmod::fmod
            fmod::fmodL
            glad::glad_static
            glfw::glfw_static
            glm::glm_static
            ImGui::ImGui_static
            spdlog::spdlog_static
            stb_image::stb_image_static
            stb_textedit::stb_textedit_static
)

install(
    TARGETS PolosThirdParty
    LIBRARY DESTINATION "${INSTALL_DIR}"
)