cmake_minimum_required(VERSION 3.24)

add_library(assimp_interface INTERFACE)
target_include_directories(assimp_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    set(assimp_libname "${CMAKE_BUILD_TYPE}/assimp.dll")
    set(assimp_implib  "${CMAKE_BUILD_TYPE}/assimp.lib")
elseif(LINUX)
#    set(assimp_libname "libassimp.so")
endif()

set(assimp_location "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${assimp_libname}")

if(WIN32)
    set(assimp_implibloc "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${assimp_implib}")
endif()

add_library(assimp SHARED IMPORTED GLOBAL)
set_target_properties(
    assimp
        PROPERTIES
            IMPORTED_LOCATION         "${assimp_location}"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
    set_target_properties(assimp PROPERTIES IMPORTED_IMPLIB "${assimp_implibloc}")
endif()
target_link_libraries(assimp INTERFACE assimp_interface)

add_library(assimp::assimp ALIAS assimp)
add_library(assimp::assimp_interface ALIAS assimp_interface)

install(
    FILES "lib/${PLATFORM_NAME}/${assimp_libname}"
    DESTINATION ${INSTALL_DIR}
)
if(LINUX)
endif()