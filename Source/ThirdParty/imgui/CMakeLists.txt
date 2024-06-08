cmake_minimum_required(VERSION 3.24)

add_library(ImGui_interface INTERFACE)
target_include_directories(ImGui_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(ImGui_static STATIC)
target_sources(
    ImGui_static
        PRIVATE
            internal/imgui_demo.cpp
            internal/imgui_draw.cpp
            internal/imgui_impl_glfw.cpp
            internal/imgui_impl_opengl3.cpp
            internal/imgui_tables.cpp
            internal/imgui_widgets.cpp
            internal/imgui.cpp
)
target_link_libraries(
    ImGui_static
        PRIVATE
            ImGui_interface
)

add_library(ImGui::ImGui_static ALIAS ImGui_static)
add_library(ImGui::ImGui_interface ALIAS ImGui_interface)
