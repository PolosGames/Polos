cpmaddpackage(
    NAME imgui
    VERSION 1.91.9
    URL https://codeload.github.com/ocornut/imgui/tar.gz/refs/tags/v1.91.9-docking
    URL_HASH SHA256=f272251572210d8089be6e5d265f9c04f84c41654ba181611a4a196484b25483
    DOWNLOAD_ONLY YES
)

add_library(
    polos_imgui
        OBJECT
            "${imgui_SOURCE_DIR}/imgui.cpp"
            "${imgui_SOURCE_DIR}/imgui_draw.cpp"
            "${imgui_SOURCE_DIR}/imgui_tables.cpp"
            "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
            "${imgui_SOURCE_DIR}/imgui_demo.cpp"
            "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
            "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
)
add_library(
    polos::imgui
        ALIAS polos_imgui
)

target_compile_features(
    polos_imgui
        PUBLIC cxx_std_23
)
target_compile_definitions(
    polos_imgui
        PRIVATE POLOS_ENGINE_EXPORTS
        PUBLIC IMGUI_USER_CONFIG="polos/gui/imgui_config.hpp"
)
target_include_directories(
    polos_imgui
        SYSTEM PUBLIC "${imgui_SOURCE_DIR}"
        PRIVATE "${POLOS_DIR}/Runtime" "${POLOS_CONFIG_INCLUDE_DIR}"
)
target_link_libraries(
    polos_imgui
        PRIVATE glfw::glfw Vulkan::Vulkan
)
set(POLOS_IMGUI_BACKEND_DIR "${imgui_SOURCE_DIR}/backends" PARENT_SCOPE)

# SDK consumers can include imgui.h without extra compiler definitions.
file(READ "${imgui_SOURCE_DIR}/imconfig.h" imgui_config)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/imconfig.h"
    "#include <polos/gui/imgui_config.hpp>\n${imgui_config}")
install(FILES
    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
    "${imgui_SOURCE_DIR}/imstb_rectpack.h"
    "${imgui_SOURCE_DIR}/imstb_textedit.h"
    "${imgui_SOURCE_DIR}/imstb_truetype.h"
    "${CMAKE_CURRENT_BINARY_DIR}/imconfig.h"
    DESTINATION "${POLOS_INSTALL_INC_DIR}"
    COMPONENT SDK
)
install(FILES "${imgui_SOURCE_DIR}/LICENSE.txt"
    DESTINATION "${POLOS_INSTALL_DIR}"
    RENAME LICENSE-ImGui.txt
    COMPONENT Runtime
)
