cmake_minimum_required(VERSION 3.24)

add_library(quill_interface INTERFACE)
target_include_directories(quill_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(quill_static STATIC IMPORTED GLOBAL)
set_target_properties(
    quill_static
        PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE}/quill.lib"
            FOLDER            "ThirdParty"
)

add_library(quill::quill_static ALIAS quill_static)
add_library(quill::quill_interface ALIAS quill_interface)
