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

# to create a unified shared library out of every static library
file(WRITE empty.cpp "")
add_library(PolosThirdParty SHARED "empty.cpp")

set(
    POLOS_TP_LIBRARIES
        "/WHOLEARCHIVE:FMOD"
        "/WHOLEARCHIVE:FMODL"
        "/WHOLEARCHIVE:glad_static"
        "/WHOLEARCHIVE:glfw_static"
        "/WHOLEARCHIVE:glm_static"
        "/WHOLEARCHIVE:ImGui_static"
        "/WHOLEARCHIVE:spdlog_static"
        "/WHOLEARCHIVE:stb_image_static"
        "/WHOLEARCHIVE:stb_textedit_static"
)
string(REPLACE ";" " " POLOS_TP_LIBRARIES_SEPERATED "${POLOS_TP_LIBRARIES}")

set_target_properties(
    PolosThirdParty
        PROPERTIES
            SHARED_LINKER_FLAGS ${POLOS_TP_LIBRARIES_SEPERATED}
)
install(
    TARGETS PolosThirdParty
    LIBRARY DESTINATION "${INSTALL_DIR}"
)