cmake_minimum_required(VERSION 3.25)

polos_sources(
    glfw_window_surface.cpp
    input_state.cpp
    platform_manager.cpp
)

polos_tests(platform SOURCES test/input_state_test.cpp)
