cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

enable_testing()
include(GoogleTest)

add_subdirectory(fixtures)

#add_subdirectory(ut/backend)
#add_subdirectory(ut/communication)
#add_subdirectory(ut/core)
#add_subdirectory(ut/datatypes)
add_subdirectory(ut/logging)
#add_subdirectory(ut/platform)
#add_subdirectory(ut/utils)
