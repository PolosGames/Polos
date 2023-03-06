cmake_minimum_required(VERSION 3.24)

add_library(fmod_interface INTERFACE)
target_include_directories(fmod_interface INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(fmod SHARED IMPORTED GLOBAL)
set_target_properties(
    fmod
        PROPERTIES
            IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/fmod.dll"
            IMPORTED_IMPLIB           "${CMAKE_CURRENT_LIST_DIR}/lib/fmod_vc.lib"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

add_library(fmodL SHARED IMPORTED GLOBAL)
set_target_properties(
    fmodL
        PROPERTIES
            IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/fmodL.dll"
            IMPORTED_IMPLIB           "${CMAKE_CURRENT_LIST_DIR}/lib/fmodL_vc.lib"
            FOLDER                    "ThirdParty"
            POSITION_INDEPENDENT_CODE ON
)

add_library(fmod::fmod ALIAS fmod)
add_library(fmod::fmodL ALIAS fmodL)
add_library(fmod::fmod_interface ALIAS fmod_interface)

install(
    FILES lib/fmod.dll
    DESTINATION ${INSTALL_DIR}
)
install(
    FILES lib/fmodL.dll
    DESTINATION ${INSTALL_DIR}
)