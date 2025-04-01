cmake_minimum_required(VERSION 3.24)

if(WIN32)
    set(PLATFORM_NAME "win32")
elseif(LINUX)
    set(PLATFORM_NAME "linux")
endif()

add_subdirectory(stb_image)
add_subdirectory(stb_textedit)

add_library(PolosThirdParty INTERFACE)

target_link_libraries(
    PolosThirdParty
        INTERFACE
            assimp::assimp
            glad::glad
            glfw
            glm::glm
            stb_image::stb_image
            stb_textedit::stb_textedit
)

target_include_directories(PolosThirdParty INTERFACE ${QUILL_INCLUDE_DIRS})

if(GUI_USE_IMGUI)
    target_link_libraries(PolosThirdParty INTERFACE imgui::imgui)
endif()
