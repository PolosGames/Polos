cmake_minimum_required(VERSION 3.24)

if(WIN32)
    set(PLATFORM_NAME "win32")
elseif(LINUX)
    set(PLATFORM_NAME "linux")
endif()

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
    target_link_libraries(ImGui_static PRIVATE glfw::glfw_interface )
endif()
add_subdirectory(quill)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)

add_library(PolosThirdParty INTERFACE)

target_link_libraries(
    PolosThirdParty
        INTERFACE
            fmod::fmod
            fmod::fmodL
            glad::glad
            glfw::glfw
            ImGui::ImGui
            quill::quill
            stb_image::stb_image
            stb_textedit::stb_textedit
)
