cmake_minimum_required(VERSION 3.24)

# ================================== #
#  Add the third parties into build  #
# ================================== #
add_subdirectory(ThirdParty)

set(POLOS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/Runtime")
set(PCH_LIBRARY_NAME RuntimePCH)

file(WRITE polos.cpp "")
add_library(${PCH_LIBRARY_NAME} OBJECT "polos.cpp")

target_precompile_headers(
    ${PCH_LIBRARY_NAME}
        PUBLIC
            polos_pch.h
)

set_target_properties(
    ${PCH_LIBRARY_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED True
)

target_link_libraries(
    ${PCH_LIBRARY_NAME}
        PRIVATE
            glm::glm_interface
            spdlog::spdlog_interface
)

target_include_directories(
    ${PCH_LIBRARY_NAME}
        PRIVATE
            ${POLOS_INCLUDE_DIR}
)

build_options(${PCH_LIBRARY_NAME} true)

add_subdirectory(Runtime)

# =================================== #
#       Build Polos with Editor       #
# =================================== #
if(BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

install(
    DIRECTORY resources
    DESTINATION ${INSTALL_DIR}
)