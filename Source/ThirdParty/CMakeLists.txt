cmake_minimum_required(VERSION 3.24)

if(WIN32)
    set(PLATFORM_NAME "win32")
elseif(LINUX)
    set(PLATFORM_NAME "linux")
endif()

add_subdirectory(quill)
add_subdirectory(stb_image)
add_subdirectory(stb_textedit)

define_polos_module(
    NAME
        third_party
    TYPE
        INTERFACE
    INTERFACE_DEPS
        assimp::assimp
        glad::glad
        glfw
        glm::glm
        imgui::imgui
        stb_image::stb_image
        stb_textedit::stb_textedit
)
