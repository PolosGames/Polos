cmake_minimum_required(VERSION 3.24)

add_library(fmod_interface INTERFACE)
target_include_directories(fmod_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    set(fmod_libname "fmod.dll")
    set(fmod_implib  "fmod_vc.lib")
    set(fmodL_libname "fmodL.dll")
    set(fmodL_implib  "fmodL_vc.lib")
elseif(LINUX)
    set(fmod_libname "libfmod.so")
    set(fmodL_libname "libfmodL.so")
endif()

set(fmod_location "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${fmod_libname}")
set(fmodL_location "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${fmodL_libname}")

if(WIN32)
    set(fmod_implibloc "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${fmod_implib}")
    set(fmodL_implibloc "${CMAKE_CURRENT_LIST_DIR}/lib/${PLATFORM_NAME}/${fmodL_implib}")
endif()

add_library(fmod SHARED IMPORTED GLOBAL)
set_target_properties(
    fmod
        PROPERTIES
            IMPORTED_LOCATION         "${fmod_location}"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

add_library(fmodL SHARED IMPORTED GLOBAL)
set_target_properties(
    fmodL
        PROPERTIES
            IMPORTED_LOCATION         "${fmodL_location}"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
    set_target_properties(fmod PROPERTIES IMPORTED_IMPLIB "${fmod_implibloc}")
    set_target_properties(fmodL PROPERTIES IMPORTED_IMPLIB "${fmodL_implibloc}")
endif()

add_library(fmod::fmod ALIAS fmod)
add_library(fmod::fmodL ALIAS fmodL)
add_library(fmod::fmod_interface ALIAS fmod_interface)

install(
    FILES "lib/${PLATFORM_NAME}/${fmod_libname}"
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES "lib/${PLATFORM_NAME}/${fmod_libname}.13"
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES "lib/${PLATFORM_NAME}/${fmod_libname}.13.22"
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES "lib/${PLATFORM_NAME}/${fmodL_libname}"
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES "lib/${PLATFORM_NAME}/${fmodL_libname}.13"
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES "lib/${PLATFORM_NAME}/${fmodL_libname}.13.22"
    DESTINATION ${INSTALL_DIR}
)