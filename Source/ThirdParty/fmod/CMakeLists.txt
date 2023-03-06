cmake_minimum_required(VERSION 3.24)

add_library(FMOD_interface INTERFACE)
target_include_directories(FMOD_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(FMOD SHARED IMPORTED)
set_target_properties(
    FMOD
        PROPERTIES
            IMPORTED_LOCATION         "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmod.dll"
            IMPORTED_IMPLIB           "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmod_vc.lib"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

add_library(FMODL SHARED IMPORTED)
set_target_properties(
    FMODL
        PROPERTIES
            IMPORTED_LOCATION         "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmodL.dll"
            IMPORTED_IMPLIB           "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmod/lib/fmodL_vc.lib"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

add_library(fmod::FMOD ALIAS FMOD)
add_library(fmod::FMODL ALIAS FMODL)
add_library(fmod::FMOD_interface ALIAS FMOD_interface)

install(
    FILES lib/fmod.dll
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES lib/fmodL.dll
    DESTINATION ${INSTALL_DIR}
)