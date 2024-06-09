cmake_minimum_required(VERSION 3.24)

file(GLOB_RECURSE PolosGUI_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB_RECURSE PolosGUI_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

add_library(
    PolosGUI STATIC
        ${PolosGUI_SOURCES} ${PolosGUI_HEADERS}
)

target_include_directories(
    PolosGUI
        PRIVATE
            ${POLOS_INCLUDE_DIR}
)

target_link_libraries(
    PolosGUI
        PRIVATE
            ImGui::ImGui
            glfw::glfw_interface
)

set_target_properties(
    PolosGUI
        PROPERTIES
            PUBLIC_HEADER             "${PolosGUI_HEADERS}"
            CXX_STANDARD              20
            CXX_STANDARD_REQUIRED     True
            POSITION_INDEPENDENT_CODE True
            OUTPUT_NAME               "PolosGUI"
)

target_precompile_headers(
    PolosGUI
        REUSE_FROM
            ${PCH_LIBRARY_NAME}
)

build_options(PolosGUI true)

install(
    TARGETS PolosGUI 
    LIBRARY       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    ARCHIVE       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    RUNTIME       DESTINATION "${POLOS_INSTALL_LIBS_DIR}"
    PUBLIC_HEADER DESTINATION "${POLOS_INSTALL_INCLUDES_DIR}/gui"
)
