cmake_minimum_required(VERSION 3.21.1)

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

if(ENABLE_LOGGING)
    add_compile_definitions(ENABLE_LOGGING)
    add_subdirectory(ThirdParty/spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty")
endif()

if(GUI_USE_IMGUI)
    add_subdirectory(ThirdParty/imgui)
    set_target_properties(ImGui PROPERTIES FOLDER "ThirdParty")
    add_compile_definitions(GUI_USE_IMGUI)
endif()

add_subdirectory(Runtime)

if(BUILD_EDITOR)
    # =================================== #
    #       Build Polos with Editor       #
    # =================================== #

    add_subdirectory(Editor)

    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Polos)
    endif()

    target_compile_features(Polos PRIVATE cxx_std_20)

    set(EXTRA_LIBS)
    if(STATIC_STD)
        if(!MSVC)
            list(APPEND EXTRA_LIBS -static-libstdc++)
        endif()
    endif()

    target_link_libraries(Polos
                          PUBLIC Runtime
                                 ${EXTRA_LIBS}
    )

    build_options(Polos true)

    install(TARGETS Polos DESTINATION ${INSTALL_DIR})
endif()