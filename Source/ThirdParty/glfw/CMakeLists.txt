cmake_minimum_required(VERSION 3.24)

add_library(glfw_interface INTERFACE)
target_include_directories(glfw_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    set(LIB_NAME "${CMAKE_BUILD_TYPE}/glfw3.dll")
    set(IMPLIB_NAME "${CMAKE_BUILD_TYPE}/glfw3dll.lib")
    set(IMPLIB_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${IMPLIB_NAME}")
elseif(LINUX)
    set(LIB_NAME "libglfw.so")
endif()

set(LIB_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${LIB_NAME}")

add_library(glfw SHARED IMPORTED GLOBAL)
set_target_properties(
    glfw
        PROPERTIES
            IMPORTED_LOCATION "${LIB_LOCATION}"
            FOLDER            "ThirdParty"
            POSITION_INDEPENDENT_CODE True
)

if(WIN32)
    set_target_properties(glfw PROPERTIES IMPORTED_IMPLIB "${IMPLIB_LOCATION}")
endif()

target_link_libraries(glfw INTERFACE glfw_interface)

add_library(glfw::glfw ALIAS glfw)
add_library(glfw::glfw_interface ALIAS glfw_interface)

install(
    FILES ${LIB_LOCATION}
    DESTINATION ${INSTALL_DIR}
)