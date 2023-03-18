cmake_minimum_required(VERSION 3.24)

add_library(ImGui_interface INTERFACE)
target_include_directories(ImGui_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(ImGui_static STATIC IMPORTED GLOBAL)
set_target_properties(
    ImGui_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/ImGui.lib"
            FOLDER            "ThirdParty"
)

add_library(ImGui::ImGui_static ALIAS ImGui_static)
add_library(ImGui::ImGui_interface ALIAS ImGui_interface)
