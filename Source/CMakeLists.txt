cmake_minimum_required(VERSION 3.24)

# ================================== #
#  Add the third parties into build  #
# ================================== #

add_subdirectory(ThirdParty/glm)
set_target_properties(glm_static   PROPERTIES FOLDER "ThirdParty")

add_subdirectory(ThirdParty/stb_image)
set_target_properties(stb_image    PROPERTIES FOLDER "ThirdParty")

add_subdirectory(ThirdParty/stb_textedit)
set_target_properties(stb_textedit PROPERTIES FOLDER "ThirdParty")

add_subdirectory(ThirdParty/rapidjson)
set_target_properties(RapidJSON    PROPERTIES FOLDER "ThirdParty")

add_subdirectory(ThirdParty/spdlog)
set_target_properties(spdlog       PROPERTIES FOLDER "ThirdParty")

if(USE_OPENGL OR USE_VULKAN)
    add_subdirectory(ThirdParty/GLFW)
    set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
endif()

if(USE_OPENGL)
    add_compile_definitions(USE_OPENGL)
    add_subdirectory(ThirdParty/glad)
    set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
endif()

if(USE_VULKAN)
    add_compile_definitions(USE_VULKAN)
endif()

if(GUI_USE_IMGUI)
    add_compile_definitions(GUI_USE_IMGUI)
    add_subdirectory(ThirdParty/imgui)
    set_target_properties(ImGui PROPERTIES FOLDER "ThirdParty")
endif()


# Specific for FMOD, will change later on
add_library(FMOD SHARED IMPORTED)
set_target_properties(
    FMOD
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmod.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmod_vc.lib"
            FOLDER "ThirdParty"
)

add_library(FMODL SHARED IMPORTED)
set_target_properties(
    FMODL
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmodL.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmodL_vc.lib"
            FOLDER "ThirdParty"
)

add_subdirectory(Runtime)

# =================================== #
#       Build Polos with Editor       #
# =================================== #
if(BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

install(DIRECTORY resources DESTINATION ${INSTALL_DIR})
install(FILES ThirdParty/fmod/lib/fmod.dll DESTINATION ${INSTALL_DIR})
install(FILES ThirdParty/fmod/lib/fmodL.dll DESTINATION ${INSTALL_DIR})