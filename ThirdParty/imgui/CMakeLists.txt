if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()

include(FetchContent)

FetchContent_Declare(polos_imgui_source
    URL https://codeload.github.com/ocornut/imgui/tar.gz/refs/tags/v1.91.9-docking
    URL_HASH SHA256=f272251572210d8089be6e5d265f9c04f84c41654ba181611a4a196484b25483
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR unused)
FetchContent_MakeAvailable(polos_imgui_source)
set(POLOS_IMGUI_SOURCE_DIR "${polos_imgui_source_SOURCE_DIR}" CACHE INTERNAL "Dear ImGui source directory")

add_library(polos_imgui OBJECT
    "${polos_imgui_source_SOURCE_DIR}/imgui.cpp"
    "${polos_imgui_source_SOURCE_DIR}/imgui_draw.cpp"
    "${polos_imgui_source_SOURCE_DIR}/imgui_tables.cpp"
    "${polos_imgui_source_SOURCE_DIR}/imgui_widgets.cpp"
    "${polos_imgui_source_SOURCE_DIR}/imgui_demo.cpp"
    "${polos_imgui_source_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${polos_imgui_source_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
)
add_library(polos::imgui ALIAS polos_imgui)

target_compile_features(polos_imgui PUBLIC cxx_std_23)
target_compile_definitions(polos_imgui
    PRIVATE POLOS_ENGINE_EXPORTS
    PUBLIC IMGUI_USER_CONFIG="polos/gui/imgui_config.hpp")
target_include_directories(polos_imgui
    PUBLIC "${polos_imgui_source_SOURCE_DIR}"
    PRIVATE "${polos_imgui_source_SOURCE_DIR}/backends"
        "${POLOS_DIR}/Runtime" "${POLOS_CONFIG_INCLUDE_DIR}")
target_link_libraries(polos_imgui PRIVATE glfw Vulkan::Vulkan)
set_target_properties(polos_imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

# SDK consumers can include imgui.h without extra compiler definitions.
file(READ "${polos_imgui_source_SOURCE_DIR}/imconfig.h" imgui_config)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/imconfig.h"
    "#include <polos/gui/imgui_config.hpp>\n${imgui_config}")
install(FILES
    "${polos_imgui_source_SOURCE_DIR}/imgui.h"
    "${polos_imgui_source_SOURCE_DIR}/imgui_internal.h"
    "${polos_imgui_source_SOURCE_DIR}/imstb_rectpack.h"
    "${polos_imgui_source_SOURCE_DIR}/imstb_textedit.h"
    "${polos_imgui_source_SOURCE_DIR}/imstb_truetype.h"
    "${CMAKE_CURRENT_BINARY_DIR}/imconfig.h"
    DESTINATION "${POLOS_INSTALL_INC_DIR}" COMPONENT SDK)
install(FILES "${polos_imgui_source_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${POLOS_INSTALL_DIR}" RENAME LICENSE-ImGui.txt COMPONENT Runtime)
